// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*! \file logger_task.h
 * \brief CLoggerTask class implements logger task.
 *
 * #Logger::CLoggerTask class is the task entity used to output or to hide all messages grouped inside one task.
 *
 * To understand task entity let discuss an example. Say, you have some function that connects to the database and
 * change some field. Like this you have many small operations like database connection, search query and add query. On
 * each step you can terminate all function (if you can't connect to the database, you won't be able to search data). In
 * case of error it would be convenient to output debug information : database and user name, database address, query
 * etc. But if function is successful, you don't need to output all this messages, even may be no messages at all. In
 * this case logger tasks will help you.
 *
 * When you call #Logger::CLoggerBase::AddTask function, you enable task mode for the current thread. During this mode all
 * messages are  collected in the internal array and they are not output. They will be output at the task finish. Before
 * task finish you set task mode - successful or not. If the task is unsuccessful, all messages will be output. In
 * another case only enabled messages will be output.
 *
 * Example below illustrates tasks usage :
 *
 * \code

// CLogger - the same class as in example above.

CLogger logger;

logger.EnableLevel( CLogger::ERROR );

{
  auto task = logger.AddTask();    // CLoggerTask task is in unsuccessful state

  logger.Output( CLogger::DEBUG, msg1 );
  logger.Output( CLogger::WARNING, msg2 );
  logger.Output( CLogger::ERROR, msg3 );

}   // At this point all messages will be output.

{
  auto task = logger.AddTask();    // CLoggerTask task is in unsuccessful state

  logger.Output( CLogger::DEBUG, msg4 );
  logger.Output( CLogger::WARNING, msg5 );
  logger.Output( CLogger::ERROR, msg6 );

  task.Succeeded(); // Now task is in successful state
}   // At this point message msg6 will be output.

 * \endcode
 *
 */

#ifndef _AVN_LOGGER_BASE_TASK_H_
#define _AVN_LOGGER_BASE_TASK_H_

#include <chrono>
#include <vector>

#include <avn/logger/data_types.h>

namespace Logger {

    template<typename _TLogData> class CLoggerTask;
    template<typename _TLogData> class ILoggerGroup;

    /** Interface for internal usage */
    template<typename _TLogData>
    class ITaskLogger{
        friend class CLoggerTask<_TLogData>;

    protected:
        virtual const TLevels& Levels() const = 0;
        virtual bool ForceAddToLog( std::size_t level, _TLogData &&data, std::chrono::system_clock::time_point time ) = 0;
        virtual void RemoveTask() = 0;

        CLoggerTask<_TLogData> CreateTask( bool init_success_state ) {
            return CLoggerTask<_TLogData>( *this, init_success_state );
        };
    };

    /** Logger task
     *
     * Task is created by #Logger::CLoggerBase::AddTask function and its overloads.
     *
     * \tparam _TLogData Logger data type. It can be string for text output, XML data field etc.
     */
    template< typename _TLogData >
    class CLoggerTask {
        friend class ITaskLogger<_TLogData>;
        friend class ILoggerGroup<_TLogData>;

    private:
        CLoggerTask( ITaskLogger<_TLogData> &logger, bool init_succeeded ) :
                _success_state( init_succeeded ), _logger( logger ), _out_levels( logger.Levels() )
            {}

    public:
        /** Logger data type */
        using TLogData = _TLogData;

        CLoggerTask( const CLoggerTask & ) = delete;
        CLoggerTask( CLoggerTask && ) = default;
        ~CLoggerTask();

        /** Set task result - success or fail */
        CLoggerTask& SetTaskResult( bool init_success_state ) { _success_state = init_success_state; return *this; }

        /** Set task result as succeeded */
        CLoggerTask& Succeeded()                        { return SetTaskResult( true ); }

        /** Set task result as failed */
        CLoggerTask& Failed()                           { return SetTaskResult( false ); }

        /** Get task result */
        bool TaskResult() const                         { return _success_state; }

        /** Output the message
         *
         * Message could be output at the task end.
         *
         * Message will be owned by logger.
         *
         * Message will be output with the current timestamp.
         *
         * \param level Message level
         * \param data Message to be output
         *
         * \return Current task instance
         */
        CLoggerTask& AddToLog( std::size_t level, _TLogData &&data );

        /** Output the message
         *
         * Message could be output at the task end.
         *
         * Message will be owned by logger.
         *
         * \param level Message level
         * \param data Message to be output
         * \param time Message time
         *
         * \return Current task instance
         */
        CLoggerTask& AddToLog( std::size_t level, _TLogData &&data, std::chrono::system_clock::time_point time );

        /** Output the message
         *
         * Message could be output at the task end.
         *
         * Message will be output with the current timestamp.
         *
         * \param level Message level
         * \param data Message to be output
         *
         * \return Current task instance
         */
        CLoggerTask& AddToLog( std::size_t level, const _TLogData &data );

        /** Output the message
         *
         * Message could be output at the task end.
         *
         * \param level Message level
         * \param data Message to be output
         *
         * \return Current task instance
         */
        CLoggerTask& AddToLog( std::size_t level, const _TLogData &data, std::chrono::system_clock::time_point time );

        /** Enable or disable specified level
         *
         * \param level Level to be enabled or disabled
         * \param to_enable To enable or disable \a level
         *
         * \return Current task instance
         */
        CLoggerTask& InitLevel( std::size_t level, bool to_enable );

        /** Enable specified levels
         *
         * \param level Levels to be enabled
         *
         * \return Current task instance
         */
        CLoggerTask& SetLevels( TLevels levels )    { _out_levels = levels; return *this; }

        /** Enable specified level
         *
         * \param level Level to be enabled
         *
         * \return Current task instance
         */
        CLoggerTask& EnableLevel( std::size_t level )   { InitLevel(level, true ); return *this; }

        /** Disable specified level
         *
         * \param level Level to be disabled
         *
         * \return Current task instance
         */
        CLoggerTask& DisableLevel( std::size_t level )  { InitLevel(level, false); return *this; }

    private:
        struct SLogEntry {
            SLogEntry( std::size_t level, _TLogData &&data, std::chrono::system_clock::time_point time ) :
                    _time(time), _level(level), _data(std::move(data)) {}

            SLogEntry( std::size_t level, const _TLogData &data, std::chrono::system_clock::time_point time ) :
                    _time(time), _level(level), _data(data) {}

            std::chrono::system_clock::time_point _time;
            std::size_t _level;
            _TLogData _data;
        };

        ITaskLogger<_TLogData>& _logger;
        TLevels _out_levels;
        std::vector<SLogEntry> _log_entries;
        bool _success_state;
    };

    template<typename _TLogData>
    CLoggerTask<_TLogData>& CLoggerTask<_TLogData>::InitLevel( std::size_t level, bool to_enable ) {
        if( to_enable ) _out_levels.emplace( level );
        else            _out_levels.erase( level );
        return *this;
    }

    template<typename _TLogData>
    CLoggerTask<_TLogData>& CLoggerTask<_TLogData>::AddToLog( std::size_t level, _TLogData &&data ) {
        _log_entries.emplace_back( level, std::move(data) );
        return *this;
    }

    template<typename _TLogData>
    CLoggerTask<_TLogData>& CLoggerTask<_TLogData>::AddToLog( std::size_t level, const _TLogData &data ) {
        _log_entries.emplace_back( level, data, std::chrono::system_clock::now() );
        return *this;
    }

    template<typename _TLogData>
    CLoggerTask<_TLogData>& CLoggerTask<_TLogData>::AddToLog( std::size_t level, _TLogData &&data, std::chrono::system_clock::time_point time ) {
        _log_entries.emplace_back( level, std::move(data), time );
        return *this;
    }

    template<typename _TLogData>
    CLoggerTask<_TLogData>& CLoggerTask<_TLogData>::AddToLog( std::size_t level, const _TLogData &data, std::chrono::system_clock::time_point time ) {
        _log_entries.emplace_back( level, data, time );
        return *this;
    }

    template<typename _TLogData>
    CLoggerTask<_TLogData>::~CLoggerTask() {
        for( auto &entry : _log_entries ) {
            if( !_success_state || _out_levels.count( entry._level ))
                _logger.ForceAddToLog( entry._level, std::move( entry._data ), entry._time );
        }

        _log_entries.clear();
        _logger.RemoveTask();
    }

} // namespace Logger

#endif  // _AVN_LOGGER_BASE_TASK_H_
