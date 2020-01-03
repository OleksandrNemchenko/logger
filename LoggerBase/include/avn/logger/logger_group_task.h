//
// Created by Oleksandr Nemchenko on 11/5/19.
//

/*! \file logger_group_task.h
 * \brief CLoggerGroupTask class implements loggers task grouping.
 *
 * #Logger::CLoggerGroupTask class is the templated container for different loggers that can be used simultaneously as one task.
 * This is some kind of #Logger::CLoggerGroup and #Logger::CLoggerTask superposition from features point of view.
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

_log.EnableLevel( WARNING );                            // Enable WARNING level
_log.AddTask();                                         // Add tasks
_log.AddString( WARNING, L"Warning message!" );         // Outputs message at the task end

 * \endcode
 *
 * In this example WARNING level is enabled and some warning message is sent simultaneously to the file and std::cout at
 * the task end.
 *
 */

#ifndef _AVN_LOGGER_LOGGER_GROUP_TASK_H
#define _AVN_LOGGER_LOGGER_GROUP_TASK_H

#include <tuple>

namespace Logger {

    /** Logger group task
     *
     * You have to call #Logger::CLoggerGroup::AddTask to create new instance and don't need to do it manually.
     *
     * \warning All loggers inside one task must have the same \a TLogData logger message data type.
     *
     * \tparam _TTaskPtr Task pointers
     */
    template< typename... _TTaskPtr >
    class CLoggerGroupTask {
    public:
        /** Task pointers arraу type */
        using TArrayPtr = std::tuple< _TTaskPtr... >;

        /** Logger data type */
        using TLogData = typename std::remove_pointer_t<std::tuple_element_t<0, TArrayPtr>>::TLogData;

        CLoggerGroupTask( const TArrayPtr & ) = delete;
        CLoggerGroupTask( TArrayPtr &&task ) : _task( std::move( task )) { }

        ~CLoggerGroupTask() { std::apply( []( auto&&... task ){ ( delete task, ... ); }, _task ); }

        /** Return task reference to the \a num element */
        template< size_t num > auto &       Task()        { static_assert( num < sizeof...( _TTaskPtr ), "Requested element's number is out of this task group size"); return *std::get<num>( _task ); }

        /** Return task reference to the \a num element */
        template< size_t num > const auto & Task() const  { static_assert( num < sizeof...( _TTaskPtr ), "Requested element's number is out of this task group size"); return *std::get<num>( _task ); }

        /** Return loggers amount */
        constexpr auto SizeOf() const                     { return sizeof...( _TTaskPtr ); }

        /** Set task result for all tasks inside container
         *
         * \param success Task result as succeeded or failed
         *
         * \return Current task group instance
         */
        CLoggerGroupTask& SetTaskResult( bool success );

        /** Set succeeded task result for all tasks inside container
         *
         * \return Current task group instance
         */
        CLoggerGroupTask& Succeeded();

        /** Set failed task result for all tasks inside container
         *
         * \return Current task group instance
         */
        CLoggerGroupTask& Failed();

        /** Add the message for all tasks inside container
         *
         * Current timestamp will be used
         *
         * \param level Message level
         * \param data Message to be output
         *
         * \return Current task group instance
         */
        CLoggerGroupTask& AddToLog( std::size_t level, const TLogData &data );

        /** Add the message for all tasks inside container
         *
         * \param level Message level
         * \param data Message to be output
         * \param time Message timestamp
         *
         * \return Current task group instance
         */
        CLoggerGroupTask& AddToLog( std::size_t level, const TLogData &data, std::chrono::system_clock::time_point time );

        /** Enable or disable specified logger level
         *
         * \param level Message level to be disabled or enabled
         * \param to_enable Enables or disabled logger level
         * \param time Message timestamp
         *
         * \return Current task group instance
         */
        CLoggerGroupTask& InitLevel( std::size_t level, bool to_enable );

        /** Enable specified logger levels
         *
         * \param levels Message levels to be enabled
         *
         * \return Current task group instance
         */
        CLoggerGroupTask& SetLevels( TLevels levels );

        /** Enable specified logger level
         *
         * \param levels Message level to be enabled
         *
         * \return Current task group instance
         */
        CLoggerGroupTask& EnableLevel( std::size_t level );

        /** Disable specified logger level
         *
         * \param levels Message level to be disabled
         *
         * \return Current task group instance
         */
        CLoggerGroupTask& DisableLevel( std::size_t level );

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
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::InitLevel( std::size_t level, bool to_enable ) {
            std::apply( [level,to_enable]( auto&... task ) { ( task->InitLevel( level, to_enable ), ... ); }, _task );
            return *this;
    }

    template< typename... _TTaskPtr >
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::SetLevels( TLevels levels ) {
            std::apply( [&levels]( auto&... task ) { ( task->SetLevels( levels ), ... ); }, _task );
            return *this;
    }

    template< typename... _TTaskPtr >
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::EnableLevel( std::size_t level ) {
            std::apply( [level]( auto&... task ) { ( task->EnableLevel( level ), ... ); }, _task );
            return *this;
    }

    template< typename... _TTaskPtr >
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::DisableLevel( std::size_t level ) {
            std::apply( [level]( auto&... task ) { ( task->DisableLevel( level ), ... ); }, _task );
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
