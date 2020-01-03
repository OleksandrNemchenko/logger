//
// Created by Oleksandr Nemchenko on 11/5/19.
//

/*! \file logger_group.h
 * \brief CLoggerGroup class implements loggers grouping.
 *
 * #Logger::CLoggerGroup class is the templated container for different loggers that can be used simultaneously, i. e. you output
 * the same logger message to different targets. For example, you can output the same message for console, disk, network etc.
 * You can enable different logger levels for each target.
 *
 * \warning All loggers inside one #Logger::CLoggerGroup must have the same \a TLogData logger message data type.
 *
 * Here is an example of #Logger::CLoggerTxtGroup that is #Logger::CLoggerGroup child for text messages :
 *
 * \code
constexpr auto WARNING;                                 // Logger level

Logger::CLoggerTxtGroup<                                // Loggers group
        Logger::CLoggerTxtCOut<true, wchar_t>,          // std::cout target
        Logger::CLoggerTxtFile<true, wchar_t>> _log;    // Text file target

const std::locale utf8_locale = locale(locale(), new codecvt_utf8<wchar_t>());

_log.Logger<1>().Imbue( utf8_locale );                  // Sets locale for text file

_log.EnableLevel( WARNING );                            // Enables WARNING levels
_log.AddString( WARNING, L"Warning message!" );         // Outputs message

 * \endcode
 *
 * In this example WARNING level is enabled and some warning message is sent simultaneously to the file and std::cout.
 *
 */

#ifndef _AVN_LOGGER_LOGGER_GROUP_H
#define _AVN_LOGGER_LOGGER_GROUP_H

#include <tuple>
#include <avn/logger/logger_base.h>
#include <avn/logger/logger_group_task.h>

namespace Logger {

    /** Interface for internal usage */
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

    /** Loggers group container
     *
     * This containter allows to manage different loggers ans send message by one call.
     *
     * \warning All loggers inside container must have the same \a TLogData logger message data type.
     *
     * \tparam _TLogger Loggers
     */
    template< typename... _TLogger >
    class CLoggerGroup {
    public:
        /** Loggers array type */
        using TArray = std::tuple<_TLogger...>;

        /** Logger data type */
        using TLogData = typename std::tuple_element_t<0, TArray>::TLogData;

        /** Return logger reference to the \a num element */
        template< size_t num > auto       & Logger()        { static_assert( num < sizeof...( _TLogger ), "Requested element's number is out of this logger group size"); return std::get<num>( _logger ); }

        /** Return logger reference to the \a num element */
        template< size_t num > const auto & Logger() const  { static_assert( num < sizeof...( _TLogger ), "Requested element's number is out of this logger group size"); return std::get<num>( _logger ); }

        /** Return loggers amount */
        constexpr auto SizeOf() const                       { return sizeof...( _TLogger ); }

        /** Enable or disable specified level for all loggers inside container */
        void InitLevel( std::size_t level, bool to_enable );

        /** Enable specified level for all loggers inside container */
        void EnableLevel( std::size_t level );

        /** Disable specified level for all loggers inside container */
        void DisableLevel( std::size_t level );

        /** Set levels for all loggers inside container */
        void SetLevels( TLevels levels );

        /** Force the message to be output for all loggers inside container
         *
         * Message will be output regardless level and task presence.
         *
         * \param level Message level
         * \param data Message to be output
         * \param time Timestamp. Current time by default
         *
         * \return true if message is output
         */
        bool ForceAddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now() );

        /** Output the message for all loggers inside container
         *
         * \param level Message level
         * \param data Message to be output
         * \param time Timestamp. Current time by default
         *
         * \return true if message is output
         */
        bool AddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now() );

        /** Add task for all loggers inside container */
        auto AddTask();

        /** Add task with \a init_success_state for all loggers inside container */
        auto AddTask( bool init_success_state );

        /** Add task with \a levels enabled for all loggers inside container */
        auto AddTask( TLevels levels );

        /** Add task with \a init_success_state and \a levels enabled for all loggers inside container */
        auto AddTask( TLevels levels, bool init_success_state );

    protected:
        TArray _logger;

    };  // class CLoggerGroup

    template< typename... _TLogger >
    void CLoggerGroup<_TLogger...>::InitLevel( std::size_t level, bool to_enable ) {
        std::apply( [=] ( auto&... logger ) {
            ( logger.InitLevel( level, to_enable ), ... );
        }, _logger );
    }

    template< typename... _TLogger >
    void CLoggerGroup<_TLogger...>::EnableLevel( std::size_t level ) {
        std::apply( [=]( auto&... logger ) {
            ( logger.EnableLevel( level ), ... );
        }, _logger );
    }

    template< typename... _TLogger >
    void CLoggerGroup<_TLogger...>::DisableLevel( std::size_t level ) {
        std::apply( [=]( auto&... logger ) {
            ( logger.DisableLevel( level ), ... );
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
