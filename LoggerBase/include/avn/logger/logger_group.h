//
// Created by Oleksandr Nemchenko on 11/5/19.
//

#ifndef _AVN_LOGGER_LOGGER_GROUP_H
#define _AVN_LOGGER_LOGGER_GROUP_H

#include <tuple>

namespace Logger {

    template< typename TLogData, typename... TLogger >
    class CLoggerGroup {
        using TArray = std::tuple<TLogger...>;

    public:
        template< size_t num > auto       & Logger()        { return std::get<num>( _logger ); }
        template< size_t num > const auto & Logger() const  { return std::get<num>( _logger ); }

        void InitLevel( std::size_t level, bool to_output );
        void OnLevel( std::size_t level );
        void OffLevel( std::size_t level );
        void SetLevels( TLevels levels );

        bool ForceAddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now() );
        bool AddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now() );

    private:
        TArray _logger;

    };  // class CLoggerGroup

    template< typename TLogData, typename... TLogger >
    void CLoggerGroup<TLogData, TLogger...>::InitLevel( std::size_t level, bool to_output ) {
        std::apply( [=] ( auto&... logger ) {
            ( logger.InitLevel( level, to_output ), ... );
            }, _logger );
    }

    template< typename TLogData, typename... TLogger >
    void CLoggerGroup<TLogData, TLogger...>::OnLevel( std::size_t level ) {
        std::apply( [=]( auto&... logger ) {
            ( logger.OnLevel( level ), ... );
        }, _logger );
    }

    template< typename TLogData, typename... TLogger >
    void CLoggerGroup<TLogData, TLogger...>::OffLevel( std::size_t level ) {
        std::apply( [=]( auto&... logger ) {
            ( logger.OffLevel( level ), ... );
        }, _logger );
    }

    template< typename TLogData, typename... TLogger >
    void CLoggerGroup<TLogData, TLogger...>::SetLevels( TLevels levels ) {
        std::apply( [&]( auto&... logger ) {
            ( logger.SetLevels( levels ), ... );
        }, _logger );
    }

    template< typename TLogData, typename... TLogger >
    bool CLoggerGroup<TLogData, TLogger...>::ForceAddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time ) {
        bool res = true;
        std::apply( [&]( auto&... logger ) {
            ( res &= ... &= logger.ForceAddToLog( level, std::move(data), time ) );
        }, _logger );
        return res;
    }

    template< typename TLogData, typename... TLogger >
    bool CLoggerGroup<TLogData, TLogger...>::AddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time ) {
        bool res = true;
        std::apply( [&]( auto&... logger ) {
            ( res &= ... &= logger.AddToLog( level, std::move(data), time ) );
        }, _logger );
        return res;
    }

}   // namespace Logger

#endif // _AVN_LOGGER_LOGGER_GROUP_H
