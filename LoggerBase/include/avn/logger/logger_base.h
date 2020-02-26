// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*! \file logger_base.h
 * \brief ALoggerBase class implements base logging functionality.
 *
 * #ALogger::ALoggerBase class is the root class for all logging functionality. It implements base entities like logger level
 * management and tasks management. You have to implement your class that inherits its
 * base functionality to introduce any unimplemented logger features.
 *
 * This class has features listed below :
 * - multithreading or single thread mode.
 * - enable or disable logger levels. If current output message has level that is enabled, it will be output. Also it
 * is possible to output regardless of current logger level by using #ALogger::ALoggerBase::forceAddToLog call.
 * - add logger tasks and automatically finish them.
 *
 * In multithread mode all output calls are protected by std::mutex. However, it is redundant for single thread mode.
 *
 * ALogger levels separate all messaged to "enabled" and "disabled" ones. During logger work (usually at the beginning)
 * you enable some logger levels. After you output messages with logger level. Like this you can disable, say, debug
 * messages for normal operation mode.
 *
 * See logger level illustration below :
 * \code

// ALogger - ALoggerBase child logger implementation with enumeration that contains DEBUG, WARNING, ERROR constants.
// It also has Output method that calls ALoggerBase::addToLog function.

ALogger logger;

logger.enableLevel(ALogger::DEBUG);
logger.enableLevel(ALogger::ERROR);

// Messages below will be output
logger.Output(ALogger::DEBUG, Msg1);
logger.Output(ALogger::ERROR, Msg2);

// Messages below won't be output
logger.Output(ALogger::WARNING, Msg3);
logger.Output(ALogger::WARNING, Msg4);

logger.disableLevel(ALogger::DEBUG);
logger.enableLevel(ALogger::WARNING);

// Messages below will be output
logger.Output(ALogger::WARNING, Msg5);
logger.Output(ALogger::ERROR, Msg6);

// Message below won't be output
logger.Output(ALogger::DEBUG, Msg7);

 * \endcode
 *
 * To understand task entity let discuss an example. Say, you have some function that connects to the database and
 * changes some field. Like this you have many small operations like database connection, search query and add query. On
 * each step you can terminate whole function (if you can't connect to the database, you won't be able to search data). In
 * case of error it would be convenient to output debug information : database and user name, database address, query
 * etc. But if function is successful, you won't need to output all this messages, even it may be no messages at all. In
 * this case logger tasks will help you.
 *
 * When you call #ALogger::ALoggerBase::addTask function, you enable task mode for the current thread. During this mode all messages are
 * collected in the internal array and without outputing. They will be output at the task finish. Before task finish
 * you set task mode - successful or not. If the task is unsuccessful, all messages will be output. In another case only
 * enabled messages will be output.
 *
 * Example below illustrates tasks usage :
 *
 * \code

// ALogger - the same class as in example above.

ALogger logger;

logger.enableLevel(ALogger::ERROR);

{
  auto task = logger.addTask();    // Task is in unsuccessful state

  logger.Output(ALogger::DEBUG, msg1);
  logger.Output(ALogger::WARNING, msg2);
  logger.Output(ALogger::ERROR, msg3);

}   // At this point all messages will be output.

{
  auto task = logger.addTask();    // Task is in unsuccessful state

  logger.Output(ALogger::DEBUG, msg4);
  logger.Output(ALogger::WARNING, msg5);
  logger.Output(ALogger::ERROR, msg6);

  task.succeeded(); // Now task is in successful state
}   // At this point message msg6 will be output.

 * \endcode
 *
 * However, during application debugging taks mode is not useful because you see messages only after task finish. You
 * can temporary or for Debug mode build disable task mode by #ALogger::ALoggerBase::disableTasks call.
 */

#ifndef _AVN_LOGGER_BASE_H_
#define _AVN_LOGGER_BASE_H_

#include <cassert>
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

namespace ALogger {

    /** Base logger functionality
     * 
     * \tparam _ThrSafe Thread security mode. If true, multithread mode will be used. Otherwise single thread mode will be activated.
     * \tparam _TLogData ALogger data type. It can be string for text output, XML data field etc.
     */
    template<bool _ThrSafe, typename _TLogData>
    class ALoggerBase :
            public ALoggerBaseThrSafety<_ThrSafe, _TLogData>,
            private ITaskLogger<_TLogData>,
            private ILoggerGroup<_TLogData>
            {
    public:

        /** Current thread security mode. If true, multithread mode will be used. Otherwise single thread mode will be activated */
        constexpr static bool ThrSafe{_ThrSafe};
        
        /** ALogger data type. It can be string for text output, XML data field etc. */
        using TLogData = _TLogData;
        
        /** Task pointers array */
        using TTasks = std::stack<ALoggerTask<_TLogData>* >;

        /** Threads map */
        using TThreads = std::map<std::thread::id, TTasks>;

        ALoggerBase() = default;
        ALoggerBase(const ALoggerBase&) = delete;
        virtual ~ALoggerBase();

        /** Levels that are used by logger
         *
         * \return The list of currently both enabled and disabled levels
         */
        const TLevels& levels() const override { return _outLevels; }

        /** Tasks in thread map
         *
         * \return The list of currently active threads with tasks
         */
        const TThreads& threadsTasks() const { return _threads; }

        /** Add task with initial state
         *
         * This call creates task for the current thread with initial \a init_success_state state. levels to be enabled
         * are inherited from current logger.
         *
         * \param[in] init_success_state Success state at the beginning. It can be changed at the end.
         *
         * \return Created task object
         */
        ALoggerTask<_TLogData> addTask(bool init_success_state);

        /** Add task with default initial state
         *
         * This call creates task for the current thread with default initial state. levels to be enabled
         * are inherited from current logger.
         *
         * \return Created task object
         */
        ALoggerTask<_TLogData> addTask() { return addTask(false); }

        /** Add task with initial state and specified levels
         *
         * This call creates task for the current thread with initial \a init_success_state state. Also it has
         * the \a levels list of levels to be enabled.
         *
         * \param[in] levels levels to be enabled.
         * \param[in] init_success_state Success state at the beginning. It can be changed later.
         *
         * \return Created task object
         */
        ALoggerTask<_TLogData> addTask(TLevels levels, bool init_success_state);

        /** Add task with default initial state and specified levels
         *
         * This call creates task for the current thread with default initial state. Also it has the \a levels list of
         * levels to be enabled.
         *
         * \param[in] levels levels to be enabled.
         *
         * \return Created task object
         */
        ALoggerTask<_TLogData> addTask(TLevels levels) { return addTask(levels, false); }

        /** Enable or disable specific level
         *
         * \param[in] level Level to be enabled or disabled.
         * \param[in] to_enable If true, \a level will be enabled. Otherwise it will be disabled.
         */
        void initLevel(std::size_t level, bool to_enable = true);

        /** Enable level
         *
         * \param[in] level Level to be enabled.
         */
        void enableLevel(std::size_t level) { initLevel(level, true); }

        /** Disable level
         *
         * \param[in] level Level to be disabled.
         */
        void disableLevel(std::size_t level) { initLevel(level, false); }

        /** Disable tasks
         *
         * This call useful for debug mode whe you need to see all messages instantly
         */
        void disableTasks() { _enableTasks = false; }

        /** Set levels
         * 
         * \param[in] levels Levels to use
         */
        void setlevels(TLevels levels) { _outLevels = levels; }

        /** Check level to be output
         *
         * \param[in] level Level to check.
         *
         * \return true if task is active or this level is enabled.
         */
        bool taskOrToBeAdded(std::size_t level) const;

        /** Force the message to be output
         *
         * Message will be output regardless level and task presence.
         *
         * Message will be output with the current timestamp.
         *
         * \param[in] level Message level
         * \param[in] data Message to be output
         *
         * \return true if message is output
         */
        bool forceAddToLog(std::size_t level, _TLogData&& data) { return forceAddToLog(level, std::move(data), std::chrono::system_clock::now()); }

        /** Force the message to be output with specified timestamp
         *
         * Message will be output regardless level and task presence.
         *
         * \param[in] level Message level
         * \param[in] data Message to be output
         * \param[in] time Timestamp
         *
         * \return true if message is output
         */
        bool forceAddToLog(std::size_t level, _TLogData&& data, std::chrono::system_clock::time_point time) override;

        /** Output the message
         *
         * If a task is active, message could be output at the task end. If no task is active, message will be output
         * only if logger level is enabled.
         *
         * Message will be output with the current timestamp.
         *
         * \param[in] level Message level
         * \param[in] data Message to be output
         *
         * \return true if message is output
         */
        bool addToLog(std::size_t level, _TLogData&& data) { return addToLog(level, std::move(data), std::chrono::system_clock::now()); }

        /** Output the message with specified timestamp
         *
         * If a task is active, message could be output at the task end. If no task is active, message will be output
         * only if logger level is enabled.
         *
         * \param[in] level Message level
         * \param[in] data Message to be output
         * \param[in] time Timestamp
         *
         * \return true if message is output
         */
        bool addToLog(std::size_t level, _TLogData&& data, std::chrono::system_clock::time_point time);

        /** Return ITaskLogger interface
         *
         * This function returns parent ITaskLogger.
         *
         * \note This function is intended to be use only for ITaskLogger friend classes.
         *
         * \return ITaskLogger interface
         */
        auto getTaskLoggerInterface() { return static_cast<ITaskLogger<TLogData>*>(this); }

        /** Return ILoggerGroup interface
         *
         * This function returns parent ILoggerGroup.
         *
         * \note This function is intended to be use only for ILoggerGroup friend classes.
         *
         * \return ILoggerGroup interface
         */
         auto getLoggerGroupInterface() { return static_cast<ILoggerGroup<TLogData>*>(this); }

    private:
        using ITask = ITaskLogger<_TLogData>;
        using IGroup = ILoggerGroup<_TLogData>;

        TLevels _outLevels;
        TThreads _threads;
        bool _enableTasks = true;

        void removeTask() override;

        ALoggerTask<_TLogData>*  addTaskForLoggerGroup(bool init_succeeded) override;
        ALoggerTask<_TLogData>*  addTaskForLoggerGroup() override                   { return addTaskForLoggerGroup(false); }
        ALoggerTask<_TLogData>*  addTaskForLoggerGroup(TLevels levels, bool init_succeeded) override;
        ALoggerTask<_TLogData>*  addTaskForLoggerGroup(TLevels levels) override     { return addTaskForLoggerGroup(levels, false); }

    };

    template<bool _ThrSafe, typename _TLogData>
    ALoggerBase<_ThrSafe, _TLogData>::~ALoggerBase()
    {
        for (auto& [thread_id, tasks] : _threads)
            assert(tasks.empty());
    }

    template<bool _ThrSafe, typename _TLogData>
    ALoggerTask<_TLogData> ALoggerBase<_ThrSafe, _TLogData>::addTask(bool init_success_state)
    {
        auto task = ITask::createTask(init_success_state);
        _threads[std::this_thread::get_id()].push(&task);
        return task;
    }

    template<bool _ThrSafe, typename _TLogData>
    ALoggerTask<_TLogData>* ALoggerBase<_ThrSafe, _TLogData>::addTaskForLoggerGroup(bool init_succeeded)
    {
        auto task = IGroup::createTask(*this, init_succeeded);
        _threads[std::this_thread::get_id()].push(task);
        return task;
    }

    template<bool _ThrSafe, typename _TLogData>
    ALoggerTask<_TLogData> ALoggerBase<_ThrSafe, _TLogData>::addTask(TLevels levels, bool init_success_state)
    {
        auto task = addTask(init_success_state);
        task.setlevels(std::forward<TLevels>(levels));
        return task;
    }

    template<bool _ThrSafe, typename _TLogData>
    ALoggerTask<_TLogData>* ALoggerBase<_ThrSafe, _TLogData>::addTaskForLoggerGroup(TLevels levels, bool init_success_state)
    {
        auto task = addTaskForLoggerGroup(init_success_state);
        task->setlevels(std::forward<TLevels>(levels));
        return task;
    }

    template<bool _ThrSafe, typename _TLogData>
    void ALoggerBase<_ThrSafe, _TLogData>::removeTask()
    {
        assert(!_threads[std::this_thread::get_id()].empty());
        _threads[std::this_thread::get_id()].pop();
    }

    template<bool _ThrSafe, typename _TLogData>
    void ALoggerBase<_ThrSafe, _TLogData>::initLevel(std::size_t level, bool to_enable)
    {
        if (to_enable)  _outLevels.emplace(level);
        else            _outLevels.erase(level);
    }

    template<bool _ThrSafe, typename _TLogData>
    bool ALoggerBase<_ThrSafe, _TLogData>::taskOrToBeAdded(std::size_t level) const
    {
        const auto thread = _threads.find(std::this_thread::get_id());
        if (thread != _threads.cend() && !thread->second.empty())
            return true;
        else if (_outLevels.count(level))
            return true;
        else
            return false;
    }

    template<bool _ThrSafe, typename _TLogData>
    bool ALoggerBase<_ThrSafe, _TLogData>::addToLog(std::size_t level, _TLogData&& data, std::chrono::system_clock::time_point time)
    {
        auto thread = _threads.find(std::this_thread::get_id());
        auto& tasks = thread->second;

        if (_enableTasks && thread != _threads.end() && !tasks.empty()) {
            auto& top = tasks.top();
            assert(top);
            top->addToLog(level, std::forward<_TLogData>(data), time);
            return true;
        } else if (taskOrToBeAdded(level)) {
            return ALoggerBaseThrSafety<_ThrSafe,_TLogData>::outDataThrSafe(level, time, move(data));
        } else {
            return false;
        }
    }

    template<bool _ThrSafe, typename _TLogData>
    bool ALoggerBase<_ThrSafe, _TLogData>::forceAddToLog(std::size_t level, _TLogData&& data, std::chrono::system_clock::time_point time)
    {
        return ALoggerBaseThrSafety<_ThrSafe,_TLogData>::outDataThrSafe(level, time, std::move(data));
    }

} // namespace ALogger

#endif  // _AVN_LOGGER_BASE_H_
