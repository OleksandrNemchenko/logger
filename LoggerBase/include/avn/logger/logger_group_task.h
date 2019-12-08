//
// Created by Oleksandr Nemchenko on 11/5/19.
//

#ifndef _AVN_LOGGER_LOGGER_GROUP_TASK_H
#define _AVN_LOGGER_LOGGER_GROUP_TASK_H

#include <tuple>

namespace Logger {

    template< typename... _TTaskPtr >
    class CLoggerGroupTask {
    public:
        using TArrayPtr = std::tuple< _TTaskPtr... >;
        using TLogData = typename std::remove_pointer_t<std::tuple_element_t<0, TArrayPtr>>::TLogData;

        CLoggerGroupTask( const TArrayPtr &) = delete;
        CLoggerGroupTask( TArrayPtr &&task ) : _task( std::move( task )) { }

        ~CLoggerGroupTask() { std::apply( []( auto&&... task ){ ( delete task, ... ); }, _task ); }

        template< size_t num > auto &       Task()        { return *std::get<num>( _task ); }
        template< size_t num > const auto & Task() const  { return *std::get<num>( _task ); }

        CLoggerGroupTask& SetTaskResult( bool success );
        CLoggerGroupTask& Succeeded();
        CLoggerGroupTask& Failed();

        CLoggerGroupTask& AddToLog( std::size_t level, const TLogData &data );
        CLoggerGroupTask& AddToLog( std::size_t level, const TLogData &data, std::chrono::system_clock::time_point time );

        CLoggerGroupTask& InitLevel( std::size_t level, bool to_output );
        CLoggerGroupTask& SetLevels( TLevels levels );
        CLoggerGroupTask& OnLevel( std::size_t level );
        CLoggerGroupTask& OffLevel( std::size_t level );

    private:
        TArrayPtr _task;

    };  // class CLoggerGroup

    template< typename... _TTaskPtr >
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::SetTaskResult( bool success ) {
        std::apply( [success]( auto&... task ) { ( task->SetTaskResult( success ), ... ); }, _task );
        return *this;
    }

    template< typename... _TTaskPtr >
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::Succeeded() {
        std::apply( []( auto&... task ) { ( task->Succeeded(), ... ); }, _task );
        return *this;
    }

    template< typename... _TTaskPtr >
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::Failed() {
        std::apply( []( auto&... task ) { ( task->Failed(), ... ); }, _task );
        return *this;
    }

    template< typename... _TTaskPtr >
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::InitLevel( std::size_t level, bool to_output ) {
            std::apply( [level,to_output]( auto&... task ) { ( task->InitLevel( level, to_output ), ... ); }, _task );
            return *this;
    }

    template< typename... _TTaskPtr >
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::SetLevels( TLevels levels ) {
            std::apply( [&levels]( auto&... task ) { ( task->SetLevels( levels ), ... ); }, _task );
            return *this;
    }

    template< typename... _TTaskPtr >
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::OnLevel( std::size_t level ) {
            std::apply( [level]( auto&... task ) { ( task->OnLevel( level ), ... ); }, _task );
            return *this;
    }

    template< typename... _TTaskPtr >
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::OffLevel( std::size_t level ) {
            std::apply( [level]( auto&... task ) { ( task->OffLevel( level ), ... ); }, _task );
            return *this;
    }

    template< typename... _TTaskPtr >
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::AddToLog( std::size_t level, const TLogData &data ) {
        std::apply( [level,data]( auto&... task ) { ( task->AddToLog( level, data ), ... ); }, _task );
        return *this;
    }

    template< typename... _TTaskPtr >
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::AddToLog( std::size_t level, const TLogData &data, std::chrono::system_clock::time_point time ) {
        std::apply( [level,data,time]( auto&... task ) { ( task->AddToLog( level, data, time ), ... ); }, _task );
        return *this;
    }


}   // namespace Logger

#endif // _AVN_LOGGER_LOGGER_GROUP_TASK_H
