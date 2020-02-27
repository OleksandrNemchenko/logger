// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*! \file logger_task.h
 * \brief ALoggerTask class implements logger task.
 *
 * #ALogger::ALoggerTask class is the task entity used to output or to hide all messages grouped inside one task.
 *
 * To understand task entity let discuss an example. Say, you have some function that connects to the database and
 * change some field. Like this you have many small operations like database connection, search query and add query. On
 * each step you can terminate all function (if you can't connect to the database, you won't be able to search data). In
 * case of error it would be convenient to output debug information : database and user name, database address, query
 * etc. But if function is successful, you don't need to output all this messages, even may be no messages at all. In
 * this case logger tasks will help you.
 *
 * When you call #ALogger::ALoggerBase::addTask function, you enable task mode for the current thread. During this mode all
 * messages are  collected in the internal array and they are not output. They will be output at the task finish. Before
 * task finish you set task mode - successful or not. If the task is unsuccessful, all messages will be output. In
 * another case only enabled messages will be output.
 *
 * Example below illustrates tasks usage :
 *
 * \code

// ALogger - the same class as in example above.

ALogger logger;

logger.enableLevel(ALogger::ERROR);

{
  auto task = logger.addTask();    // ALoggerTask task is in unsuccessful state

  logger.Output(ALogger::DEBUG, msg1);
  logger.Output(ALogger::WARNING, msg2);
  logger.Output(ALogger::ERROR, msg3);

}   // At this point all messages will be output.

{
  auto task = logger.addTask();    // ALoggerTask task is in unsuccessful state

  logger.Output(ALogger::DEBUG, msg4);
  logger.Output(ALogger::WARNING, msg5);
  logger.Output(ALogger::ERROR, msg6);

  task.succeeded(); // Now task is in successful state
}   // At this point message msg6 will be output.

 * \endcode
 *
 */

#ifndef _AVN_LOGGER_BASE_TASK_H_
#define _AVN_LOGGER_BASE_TASK_H_

#include <chrono>
#include <vector>

#include <avn/logger/data_types.h>

namespace ALogger {

    template<typename _TLogData> class ALoggerTask;
    template<typename _TLogData> class ILoggerGroup;

    /** Interface for internal usage */
    template<typename _TLogData>
    class ITaskLogger{
        friend class ALoggerTask<_TLogData>;

    protected:
        ALoggerTask<_TLogData> createTask(bool init_success_state) noexcept
        {
            return ALoggerTask<_TLogData>(*this, init_success_state);
        };

    private:
        virtual const TLevels& levels() const noexcept = 0;
        virtual bool forceAddToLog(std::size_t level, const _TLogData& data, std::chrono::system_clock::time_point time) noexcept = 0;
        virtual void removeTask() noexcept = 0;
    };

    /** ALogger task
     *
     * Task is created by #ALogger::ALoggerBase::addTask function and its overloads.
     *
     * \tparam _TLogData ALogger data type. It can be string for text output, XML data field etc.
     */
    template< typename _TLogData >
    class ALoggerTask {
        friend class ITaskLogger<_TLogData>;
        friend class ILoggerGroup<_TLogData>;

    private:
        ALoggerTask(ITaskLogger<_TLogData>& logger, bool init_succeeded) noexcept :
                _successState(init_succeeded), _logger(logger), _outLevels(logger.levels())
            {}

    public:
        /** ALogger data type */
        using TLogData = _TLogData;

        ALoggerTask() = delete;
        ALoggerTask(const ALoggerTask&) = delete;
        ALoggerTask(ALoggerTask&&) noexcept = default;

        ALoggerTask operator=(const ALoggerTask&) = delete;
        ALoggerTask operator=(ALoggerTask&&) = delete;

        ~ALoggerTask() noexcept;

        /** Set task result - success or fail */
        ALoggerTask& setTaskResult(bool init_success_state) noexcept { _successState = init_success_state; return *this; }

        /** Set task result as succeeded */
        ALoggerTask& succeeded() noexcept                            { return setTaskResult(true); }

        /** Set task result as failed */
        ALoggerTask& failed() noexcept                               { return setTaskResult(false); }

        /** Get task result */
        bool TaskResult() const noexcept                             { return _successState; }

        /** Output the message
         *
         * Message could be output at the task end.
         *
         * Message will be owned by logger.
         *
         * \param[in] level Message level
         * \param[in] data Message to be output
         * \param[in] time Message time. Current timestamp by default
         *
         * \return Current task instance
         */
        template<typename TData>
        ALoggerTask& addToLog(std::size_t level, TData data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now()) noexcept;

        /** Enable or disable specified level
         *
         * \param[in] level Level to be enabled or disabled
         * \param[in] to_enable To enable or disable \a level
         *
         * \return Current task instance
         */
        ALoggerTask& initLevel(std::size_t level, bool to_enable) noexcept;

        /** Enable specified levels
         *
         * \param[in] level levels to be enabled
         *
         * \return Current task instance
         */
        ALoggerTask& setlevels(TLevels levels) noexcept   { _outLevels = levels; return *this; }

        /** Enable specified level
         *
         * \param[in] level Level to be enabled
         *
         * \return Current task instance
         */
        ALoggerTask& enableLevel(std::size_t level) noexcept  { initLevel(level, true); return *this; }

        /** Disable specified level
         *
         * \param[in] level Level to be disabled
         *
         * \return Current task instance
         */
        ALoggerTask& disableLevel(std::size_t level) noexcept { initLevel(level, false); return *this; }

    private:
        struct SLogEntry {
            template<typename TData>
            SLogEntry(std::size_t level, TData data, std::chrono::system_clock::time_point time) noexcept :
                    _time(time), _level(level), _data(std::forward<TData>(data)) {}

            std::chrono::system_clock::time_point _time;
            std::size_t _level;
            _TLogData _data;
        };

        ITaskLogger<_TLogData>& _logger;
        TLevels _outLevels;
        std::vector<SLogEntry> _logEntries;
        bool _successState;
    };

    template<typename _TLogData>
    ALoggerTask<_TLogData>& ALoggerTask<_TLogData>::initLevel(std::size_t level, bool to_enable) noexcept
    {
        if (to_enable) _outLevels.emplace(level);
        else           _outLevels.erase(level);
        return *this;
    }

    template<typename _TLogData>
    template<typename TData>
    ALoggerTask<_TLogData>& ALoggerTask<_TLogData>::addToLog(std::size_t level, TData data, std::chrono::system_clock::time_point time) noexcept
    {
        _logEntries.emplace_back(level, std::forward<TData>(data), time);
        return *this;
    }

    template<typename _TLogData>
    ALoggerTask<_TLogData>::~ALoggerTask() noexcept
    {
        for (auto& entry : _logEntries) {
            if (!_successState || _outLevels.count(entry._level ))
                _logger.forceAddToLog(entry._level, std::move(entry._data), entry._time);
        }

        _logEntries.clear();
        _logger.removeTask();
    }

} // namespace ALogger

#endif  // _AVN_LOGGER_BASE_TASK_H_
