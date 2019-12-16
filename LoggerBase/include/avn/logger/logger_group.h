//
// Created by Oleksandr Nemchenko on 11/5/19.
//

#ifndef _AVN_LOGGER_LOGGER_GROUP_H
#define _AVN_LOGGER_LOGGER_GROUP_H

#include <tuple>
#include <avn/logger/logger_base.h>
#include <avn/logger/logger_group_task.h>

namespace Logger {

    template< typename _TLogData >
    class ILoggerGroup {
        template< typename... _TLogger >
        friend class CLoggerGroup;

    protected:
        virtual CLoggerTask<_TLogData> * AddTaskForLoggerGroup( bool init_succeeded ) = 0;
        virtual CLoggerTask<_TLogData> * AddTaskForLoggerGroup() = 0;
        virtual CLoggerTask<_TLogData> * AddTaskForLoggerGroup( TLevels levels, bool init_succeeded ) = 0;
        virtual CLoggerTask<_TLogData> * AddTaskForLoggerGroup( TLevels levels ) = 0;

        CLoggerTask<_TLogData> * CreateTask( ITaskLogger<_TLogData> &logger, bool init_success_state ) {
            return new CLoggerTask<_TLogData>( logger, init_success_state );
        };

    };  // class CLoggerGroup

    template< typename... _TLogger >
    class CLoggerGroup {
    public:
        using TArray = std::tuple<_TLogger...>;
        using TLogData = typename std::tuple_element_t<0, TArray>::TLogData;

        template< size_t num > auto       & Logger()        { return std::get<num>( _logger ); }
        template< size_t num > const auto & Logger() const  { return std::get<num>( _logger ); }
        constexpr auto SizeOf() const                       { return sizeof...( _TLogger ); }

        void InitLevel( std::size_t level, bool to_output );
        void OnLevel( std::size_t level );
        void OffLevel( std::size_t level );
        void SetLevels( TLevels levels );

        bool ForceAddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now() );
        bool AddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now() );

        auto AddTask();
        auto AddTask( bool init_success_state );
        auto AddTask( TLevels levels );
        auto AddTask( TLevels levels, bool init_success_state );

    protected:
        TArray _logger;

    };  // class CLoggerGroup

    template< typename... _TLogger >
    void CLoggerGroup<_TLogger...>::InitLevel( std::size_t level, bool to_output ) {
        std::apply( [=] ( auto&... logger ) {
            ( logger.InitLevel( level, to_output ), ... );
        }, _logger );
    }

    template< typename... _TLogger >
    void CLoggerGroup<_TLogger...>::OnLevel( std::size_t level ) {
        std::apply( [=]( auto&... logger ) {
            ( logger.OnLevel( level ), ... );
        }, _logger );
    }

    template< typename... _TLogger >
    void CLoggerGroup<_TLogger...>::OffLevel( std::size_t level ) {
        std::apply( [=]( auto&... logger ) {
            ( logger.OffLevel( level ), ... );
        }, _logger );
    }

    template< typename... _TLogger >
    void CLoggerGroup<_TLogger...>::SetLevels( TLevels levels ) {
        std::apply( [&]( auto&... logger ) { ( logger.SetLevels( levels ), ... ); }, _logger );
    }

    template< typename... _TLogger >
    bool CLoggerGroup<_TLogger...>::ForceAddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time ) {
        bool res = true;
        std::apply( [&]( auto&... logger ) { ( res &= ... &= logger.ForceAddToLog( level, std::move(data), time ) ); }, _logger );
        return res;
    }

    template< typename... _TLogger >
    bool CLoggerGroup<_TLogger...>::AddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time ) {
        bool res = true;
        std::apply( [&]( auto&... logger ) { ( res &= ... &= logger.AddToLog( level, std::move(data), time ) ); }, _logger );
        return res;
    }

    template< typename... _TLogger >
    auto CLoggerGroup<_TLogger...>::AddTask() {
        auto tasks = std::apply( []( auto&&... logger ){
            return std::make_tuple( (logger.GetLoggerGroupInterface())->AddTaskForLoggerGroup() ... );
        }, _logger );
        return CLoggerGroupTask( std::move( tasks ));
    }

    template< typename... _TLogger >
    auto CLoggerGroup<_TLogger...>::AddTask( bool init_success_state ) {
        auto tasks = std::apply( [init_success_state]( auto&&... logger ){
            return std::make_tuple( (logger.GetLoggerGroupInterface())->AddTaskForLoggerGroup( init_success_state ) ... );
        }, _logger );
        return CLoggerGroupTask( std::move( tasks ));
    }

    template< typename... _TLogger >
    auto CLoggerGroup<_TLogger...>::AddTask( TLevels levels ) {
        auto tasks = std::apply( [&levels]( auto&&... logger ){
            return std::make_tuple( (logger.GetLoggerGroupInterface())->AddTaskForLoggerGroup( levels ) ... );
        }, _logger );
        return CLoggerGroupTask( std::move( tasks ));
    }

    template< typename... _TLogger >
    auto CLoggerGroup<_TLogger...>::AddTask( TLevels levels, bool init_success_state ) {
        auto tasks = std::apply( [&levels,init_success_state]( auto&&... logger ){
            return std::make_tuple( (logger.GetLoggerGroupInterface())->AddTaskForLoggerGroup( levels, init_success_state ) ... );
        }, _logger );
        return CLoggerGroupTask( std::move( tasks ));
    }

}   // namespace Logger

#endif // _AVN_LOGGER_LOGGER_GROUP_H
