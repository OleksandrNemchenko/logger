// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*! \file logger_base.h
 * \brief CLoggerBase class implements base logging functionality.
 *
 * #Logger::CLoggerBase class is the root class for all logging functionality. It implements base entities like logger level
 * management and tasks management. You have to implement your class that inherits its
 * base functionality to introduce any unimplemented logger features.
 *
 * This class has features listed below :
 * - multithreading or single thread mode.
 * - enable or disable logger levels. If current output message has level that is enabled, it will be output. Also it
 * is possible to output regardless of current logger level by using #Logger::CLoggerBase::ForceAddToLog call.
 * - add logger tasks and automatically finish them.
 *
 * In multithread mode all output calls are protected by std::mutex. However, it is redundant for single thread mode.
 *
 * Logger levels separate all messaged to "enabled" and "disabled" ones. During logger work (usually at the beginning)
 * you enable some logger levels. After you output messages with logger level. Like this you can disable, say, debug
 * messages for normal operation mode.
 *
 * See logger level illustration below :
 * \code

// CLogger - CLoggerBase child logger implementation with enumeration that contains DEBUG, WARNING, ERROR constants.
// It also has Output method that calls CLoggerBase::AddToLog function.

CLogger logger;

logger.EnableLevel( CLogger::DEBUG );
logger.EnableLevel( CLogger::ERROR );

// Messages below will be output
logger.Output( CLogger::DEBUG, Msg1 );
logger.Output( CLogger::ERROR, Msg2 );

// Messages below won't be output
logger.Output( CLogger::WARNING, Msg3 );
logger.Output( CLogger::WARNING, Msg4 );

logger.DisableLevel( CLogger::DEBUG );
logger.EnableLevel( CLogger::WARNING );

// Messages below will be output
logger.Output( CLogger::WARNING, Msg5 );
logger.Output( CLogger::ERROR, Msg6 );

// Message below won't be output
logger.Output( CLogger::DEBUG, Msg7 );

 * \endcode
 *
 * To understand task entity let discuss an example. Say, you have some function that connects to the database and
 * changes some field. Like this you have many small operations like database connection, search query and add query. On
 * each step you can terminate whole function (if you can't connect to the database, you won't be able to search data). In
 * case of error it would be convenient to output debug information : database and user name, database address, query
 * etc. But if function is successful, you won't need to output all this messages, even it may be no messages at all. In
 * this case logger tasks will help you.
 *
 * When you call #Logger::CLoggerBase::AddTask function, you enable task mode for the current thread. During this mode all messages are
 * collected in the internal array and without outputing. They will be output at the task finish. Before task finish
 * you set task mode - successful or not. If the task is unsuccessful, all messages will be output. In another case only
 * enabled messages will be output.
 *
 * Example below illustrates tasks usage :
 *
 * \code

// CLogger - the same class as in example above.

CLogger logger;

logger.EnableLevel( CLogger::ERROR );

{
  auto task = logger.AddTask();    // Task is in unsuccessful state

  logger.Output( CLogger::DEBUG, msg1 );
  logger.Output( CLogger::WARNING, msg2 );
  logger.Output( CLogger::ERROR, msg3 );

}   // At this point all messages will be output.

{
  auto task = logger.AddTask();    // Task is in unsuccessful state

  logger.Output( CLogger::DEBUG, msg4 );
  logger.Output( CLogger::WARNING, msg5 );
  logger.Output( CLogger::ERROR, msg6 );

  task.Succeeded(); // Now task is in successful state
}   // At this point message msg6 will be output.

 * \endcode
 */

#ifndef _AVN_LOGGER_BASE_H_
#define _AVN_LOGGER_BASE_H_

#include <chrono>
#include <map>
#include <set>
#include <stack>
#include <thread>
#include <vector>

#include <avn/logger/data_types.h>
#include <avn/logger/base_thr_safety.h>
#include <avn/logger/logger_task.h>
#include <avn/logger/logger_group.h>

namespace Logger {

    /** Base logger functionality
     * 
     * \tparam _ThrSafe Thread security mode. If true, multithread mode will be used. Otherwise single thread mode will be activated.
     * \tparam _TLogData Logger data type. It can be string for text output, XML data field etc.
     */
    template<bool _ThrSafe, typename _TLogData>
    class CLoggerBase :
            public CLoggerBaseThrSafety<_ThrSafe, _TLogData>,
            private ITaskLogger<_TLogData>,
            private ILoggerGroup<_TLogData>
            {
    public:

        /** Current thread security mode. If true, multithread mode will be used. Otherwise single thread mode will be activated */
        constexpr static bool ThrSafe{ _ThrSafe };
        
        /** Logger data type. It can be string for text output, XML data field etc. */
        using TLogData = _TLogData;
        
        /** Task pointers array */
        using TTasks = std::stack<CLoggerTask<_TLogData> *>;

        /** Threads map */
        using TThreads = std::map<std::thread::id, TTasks>;

        CLoggerBase() = default;
        CLoggerBase( const CLoggerBase & ) = delete;
        virtual ~CLoggerBase();

        /** Levels that are used by logger
         *
         * \return The list of currently both enabled and disabled levels
         */
        const TLevels& Levels() const override { return _out_levels; }

        /** Tasks in thread map
         *
         * \return The list of currently active threads with tasks
         */
        const TThreads& ThreadsTasks() const { return _threads; }

        /** Add task with initial state
         *
         * This call creates task for the current thread with initial \a init_success_state state. Levels to be enabled
         * are inherited from current logger.
         *
         * \param init_success_state Success state at the beginning. It can be changed at the end.
         *
         * \return Created task object
         */
        CLoggerTask<_TLogData> AddTask( bool init_success_state );

        /** Add task with default initial state
         *
         * This call creates task for the current thread with default initial state. Levels to be enabled
         * are inherited from current logger.
         *
         * \return Created task object
         */
        CLoggerTask<_TLogData> AddTask()                   { return AddTask( false ); }

        /** Add task with initial state and specified levels
         *
         * This call creates task for the current thread with initial \a init_success_state state. Also it has
         * the \a levels list of levels to be enabled.
         *
         * \param levels Levels to be enabled.
         * \param init_success_state Success state at the beginning. It can be changed later.
         *
         * \return Created task object
         */
        CLoggerTask<_TLogData> AddTask( TLevels levels, bool init_success_state );

        /** Add task with default initial state and specified levels
         *
         * This call creates task for the current thread with default initial state. Also it has the \a levels list of
         * levels to be enabled.
         *
         * \param levels Levels to be enabled.
         *
         * \return Created task object
         */
        CLoggerTask<_TLogData> AddTask( TLevels levels )   { return AddTask( levels, false ); }

        /** Enable or disable specific level
         *
         * \param level Level to be enabled or disabled.
         * \param to_enable If true, \a level will be enabled. Otherwise it will be disabled.
         */
        void InitLevel( std::size_t level, bool to_enable = true );

        /** Enable level
         *
         * \param level Level to be enabled.
         */
        void EnableLevel( std::size_t level )  { InitLevel(level, true); }

        /** Disable level
         *
         * \param level Level to be disabled.
         */        void DisableLevel( std::size_t level ) { InitLevel(level, false); }
        void SetLevels( TLevels levels )   { _out_levels = levels; }

        /** Check level to be output
         *
         * \param level Level to check.
         * @return true if task is active or this level is enabled.
         */
        bool TaskOrToBeAdded( std::size_t level ) const;

        /** Force the message to be output
         *
         * Message will be output regardless level and task presence.
         *
         * Message will be output with the current timestamp.
         *
         * \param level Message level
         * \param data Message to be output
         *
         * \return true if message is output
         */
        bool ForceAddToLog( std::size_t level, _TLogData &&data )   { return ForceAddToLog( level, std::move( data ), std::chrono::system_clock::now() ); }

        /** Force the message to be output with specified timestamp
         *
         * Message will be output regardless level and task presence.
         *
         * \param level Message level
         * \param data Message to be output
         * \param time Timestamp
         *
         * \return true if message is output
         */
        bool ForceAddToLog( std::size_t level, _TLogData &&data, std::chrono::system_clock::time_point time ) override;

        /** Output the message
         *
         * If a task is active, message could be output at the task end. If no task is active, message will be output
         * only if logger level is enabled.
         *
         * Message will be output with the current timestamp.
         *
         * \param level Message level
         * \param data Message to be output
         *
         * \return true if message is output
         */
        bool AddToLog( std::size_t level, _TLogData &&data)     { return AddToLog( level, std::move(data), std::chrono::system_clock::now() ); }

        /** Output the message with specified timestamp
         *
         * If a task is active, message could be output at the task end. If no task is active, message will be output
         * only if logger level is enabled.
         *
         * \param level Message level
         * \param data Message to be output
         * \param time Timestamp
         *
         * \return true if message is output
         */
        bool AddToLog( std::size_t level, _TLogData &&data, std::chrono::system_clock::time_point time );

        /** Return ITaskLogger interface
         *
         * This function returns parent ITaskLogger.
         *
         * \note This function is intended to be use only for ITaskLogger friend classes.
         *
         * \return ITaskLogger interface
         */
        auto GetTaskLoggerInterface()   { return static_cast<ITaskLogger<TLogData>*>( this ); }

        /** Return ILoggerGroup interface
         *
         * This function returns parent ILoggerGroup.
         *
         * \note This function is intended to be use only for ILoggerGroup friend classes.
         *
         * \return ILoggerGroup interface
         */
         auto GetLoggerGroupInterface()  { return static_cast<ILoggerGroup<TLogData>*>( this ); }

    private:
        using ITask = ITaskLogger<_TLogData>;
        using IGroup = ILoggerGroup<_TLogData>;

        TLevels _out_levels;
        TThreads _threads;

        void RemoveTask() override;

        CLoggerTask<_TLogData> * AddTaskForLoggerGroup( bool init_succeeded ) override;
        CLoggerTask<_TLogData> * AddTaskForLoggerGroup() override                    { return AddTaskForLoggerGroup( false ); }
        CLoggerTask<_TLogData> * AddTaskForLoggerGroup( TLevels levels, bool init_succeeded ) override;
        CLoggerTask<_TLogData> * AddTaskForLoggerGroup( TLevels levels ) override    { return AddTaskForLoggerGroup( levels, false ); }

    };

    template<bool _ThrSafe, typename _TLogData>
    CLoggerBase<_ThrSafe, _TLogData>::~CLoggerBase(){
        for( auto &[thread_id, tasks] : _threads )
            assert( tasks.empty() );
    }

    template<bool _ThrSafe, typename _TLogData>
    CLoggerTask<_TLogData> CLoggerBase<_ThrSafe, _TLogData>::AddTask( bool init_success_state ) {
        auto task = ITask::CreateTask( init_success_state );
        _threads[std::this_thread::get_id()].push(&task);
        return task;
    }

    template<bool _ThrSafe, typename _TLogData>
    CLoggerTask<_TLogData> *CLoggerBase<_ThrSafe, _TLogData>::AddTaskForLoggerGroup( bool init_succeeded ) {
        auto task = IGroup::CreateTask( *this, init_succeeded );
        _threads[std::this_thread::get_id()].push(task);
        return task;
    }

    template<bool _ThrSafe, typename _TLogData>
    CLoggerTask<_TLogData> CLoggerBase<_ThrSafe, _TLogData>::AddTask(TLevels levels, bool init_success_state ) {
        auto task = AddTask( init_success_state );
        task.SetLevels( std::forward<TLevels>(levels) );
        return task;
    }

    template<bool _ThrSafe, typename _TLogData>
    CLoggerTask<_TLogData> * CLoggerBase<_ThrSafe, _TLogData>::AddTaskForLoggerGroup(TLevels levels, bool init_success_state ) {
        auto task = AddTaskForLoggerGroup( init_success_state );
        task->SetLevels( std::forward<TLevels>(levels) );
        return task;
    }

    template<bool _ThrSafe, typename _TLogData>
    void CLoggerBase<_ThrSafe, _TLogData>::RemoveTask() {
        assert(!_threads[std::this_thread::get_id()].empty());
        _threads[std::this_thread::get_id()].pop();
    }

    template<bool _ThrSafe, typename _TLogData>
    void CLoggerBase<_ThrSafe, _TLogData>::InitLevel( std::size_t level, bool to_enable ) {
        if( to_enable ) _out_levels.emplace( level );
        else            _out_levels.erase( level );
    }

    template<bool _ThrSafe, typename _TLogData>
    bool CLoggerBase<_ThrSafe, _TLogData>::TaskOrToBeAdded( std::size_t level ) const {
        const auto thread = _threads.find(std::this_thread::get_id());
        if( thread != _threads.cend() && !thread->second.empty() )
            return true;
        else if ( _out_levels.count(level) )
            return true;
        else
            return false;
    }

    template<bool _ThrSafe, typename _TLogData>
    bool CLoggerBase<_ThrSafe, _TLogData>::AddToLog( std::size_t level, _TLogData &&data, std::chrono::system_clock::time_point time ) {
        auto thread = _threads.find(std::this_thread::get_id());
        auto &tasks = thread->second;
        if( thread != _threads.end() && !tasks.empty() ) {
            auto &top = tasks.top();
            assert( top );
            top->AddToLog( level, std::forward<_TLogData>(data), time );
            return true;
        } else if( TaskOrToBeAdded(level) )
            return CLoggerBaseThrSafety<_ThrSafe,_TLogData>::OutDataThrSafe( level, time, move(data) );
        else
            return false;
    }

    template<bool _ThrSafe, typename _TLogData>
    bool CLoggerBase<_ThrSafe, _TLogData>::ForceAddToLog( std::size_t level, _TLogData &&data, std::chrono::system_clock::time_point time ) {
        return CLoggerBaseThrSafety<_ThrSafe,_TLogData>::OutDataThrSafe( level, time, std::move(data) );
    }

} // namespace Logger

#endif  // _AVN_LOGGER_BASE_H_
