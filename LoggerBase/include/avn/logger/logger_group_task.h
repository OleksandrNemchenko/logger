//
// Created by Oleksandr Nemchenko on 11/5/19.
//

/*! \file logger_group_task.h
 * \brief ALoggerGroupTask class implements loggers task grouping.
 *
 * #ALogger::ALoggerGroupTask class is the templated container for different loggers that can be used simultaneously as one task.
 * This is some kind of #ALogger::ALoggerGroup and #ALogger::ALoggerTask superposition from features point of view.
 *
 * \warning All loggers inside one #ALogger::ALoggerGroup must have the same \a TLogData logger message data type.
 *
 * Here is an example of #ALogger::ALoggerTxtGroup that is #ALogger::ALoggerGroup child for text messages :
 *
 * \code
constexpr auto WARNING;                                 // ALogger level

ALogger::ALoggerTxtGroup<                                // Loggers group
        ALogger::ALoggerTxtCOut<true, wchar_t>,          // std::cout target
        ALogger::ALoggerTxtFile<true, wchar_t>> _log;    // Text file target

const std::locale utf8_locale = locale(locale(), new codecvt_utf8<wchar_t>());

_log.ALogger<1>().imbue(utf8_locale);                  // Sets locale for text file

_log.enableLevel(WARNING);                            // Enable WARNING level
_log.addTask();                                         // Add tasks
_log.addString(WARNING, L"Warning message!");         // Outputs message at the task end

 * \endcode
 *
 * In this example WARNING level is enabled and some warning message is sent simultaneously to the file and std::cout at
 * the task end.
 *
 */

#ifndef _AVN_LOGGER_LOGGER_GROUP_TASK_H
#define _AVN_LOGGER_LOGGER_GROUP_TASK_H

#include <tuple>

namespace ALogger {

    /** ALogger group task
     *
     * You have to call #ALogger::ALoggerGroup::addTask to create new instance and don't need to do it manually.
     *
     * \warning All loggers inside one task must have the same \a TLogData logger message data type.
     *
     * \tparam _TTaskPtr Task pointers
     */
    template< typename... _TTaskPtr >
    class ALoggerGroupTask {
    public:
        /** Task pointers arraу type */
        using TArrayPtr = std::tuple< _TTaskPtr... >;

        /** ALogger data type */
        using TLogData = typename std::remove_pointer_t<std::tuple_element_t<0, TArrayPtr>>::TLogData;

        ALoggerGroupTask(const TArrayPtr&) = delete;
        ALoggerGroupTask(TArrayPtr&& task) noexcept : _task(std::move(task )) { }

        ~ALoggerGroupTask() noexcept { std::apply([](auto&&... task){ (delete task, ...); }, _task); }

        /** Return task reference to the \a num element */
        template< size_t num > auto& task() noexcept                { static_assert(num < sizeof...(_TTaskPtr), "Requested element's number is out of this task group size"); return *std::get<num>(_task); }

        /** Return task reference to the \a num element */
        template< size_t num > const auto& task() const noexcept    { static_assert(num < sizeof...(_TTaskPtr), "Requested element's number is out of this task group size"); return *std::get<num>(_task); }

        /** Return loggers amount */
        constexpr auto sizeOf() const noexcept                      { return sizeof...(_TTaskPtr); }

        /** Set task result for all tasks inside container
         *
         * \param[in] success Task result as succeeded or failed
         *
         * \return Current task group instance
         */
        ALoggerGroupTask& setTaskResult(bool success) noexcept;

        /** Set succeeded task result for all tasks inside container
         *
         * \return Current task group instance
         */
        ALoggerGroupTask& succeeded() noexcept;

        /** Set failed task result for all tasks inside container
         *
         * \return Current task group instance
         */
        ALoggerGroupTask& failed() noexcept;

        /** Add the message for all tasks inside container
         *
         * Current timestamp will be used
         *
         * \param[in] level Message level
         * \param[in] data Message to be output
         *
         * \return Current task group instance
         */
        ALoggerGroupTask& addToLog(std::size_t level, const TLogData& data) noexcept;

        /** Add the message for all tasks inside container
         *
         * \param[in] level Message level
         * \param[in] data Message to be output
         * \param[in] time Message timestamp
         *
         * \return Current task group instance
         */
        ALoggerGroupTask& addToLog(std::size_t level, const TLogData& data, std::chrono::system_clock::time_point time) noexcept;

        /** Enable or disable specified logger level
         *
         * \param[in] level Message level to be disabled or enabled
         * \param[in] to_enable Enables or disabled logger level
         * \param[in] time Message timestamp
         *
         * \return Current task group instance
         */
        ALoggerGroupTask& initLevel(std::size_t level, bool to_enable) noexcept;

        /** Enable specified logger levels
         *
         * \param[in] levels Message levels to be enabled
         *
         * \return Current task group instance
         */
        ALoggerGroupTask& setlevels(TLevels levels) noexcept;

        /** Enable specified logger level
         *
         * \param[in] levels Message level to be enabled
         *
         * \return Current task group instance
         */
        ALoggerGroupTask& enableLevel(std::size_t level) noexcept;

        /** Disable specified logger level
         *
         * \param[in] levels Message level to be disabled
         *
         * \return Current task group instance
         */
        ALoggerGroupTask& disableLevel(std::size_t level) noexcept;

    private:
        TArrayPtr _task;

    };  // class ALoggerGroup

    template< typename... _TTaskPtr >
    ALoggerGroupTask<_TTaskPtr...>& ALoggerGroupTask<_TTaskPtr...>::setTaskResult(bool success) noexcept
    {
        std::apply([success](auto&... task) { (task->setTaskResult(success), ...); }, _task);
        return *this;
    }

    template< typename... _TTaskPtr >
    ALoggerGroupTask<_TTaskPtr...>& ALoggerGroupTask<_TTaskPtr...>::succeeded() noexcept
    {
        std::apply([](auto&... task) { (task->succeeded(), ...); }, _task);
        return *this;
    }

    template< typename... _TTaskPtr >
    ALoggerGroupTask<_TTaskPtr...>& ALoggerGroupTask<_TTaskPtr...>::failed() noexcept
    {
        std::apply([](auto&... task) { (task->failed(), ...); }, _task);
        return *this;
    }

    template< typename... _TTaskPtr >
    ALoggerGroupTask<_TTaskPtr...>& ALoggerGroupTask<_TTaskPtr...>::initLevel(std::size_t level, bool to_enable) noexcept
    {
            std::apply([level,to_enable](auto&... task) { (task->initLevel(level, to_enable), ...); }, _task);
            return *this;
    }

    template< typename... _TTaskPtr >
    ALoggerGroupTask<_TTaskPtr...>& ALoggerGroupTask<_TTaskPtr...>::setlevels(TLevels levels) noexcept
    {
            std::apply([&levels](auto&... task) { (task->setlevels(levels), ...); }, _task);
            return *this;
    }

    template< typename... _TTaskPtr >
    ALoggerGroupTask<_TTaskPtr...>& ALoggerGroupTask<_TTaskPtr...>::enableLevel(std::size_t level) noexcept
    {
            std::apply([level](auto&... task) { (task->enableLevel(level), ...); }, _task);
            return *this;
    }

    template< typename... _TTaskPtr >
    ALoggerGroupTask<_TTaskPtr...>& ALoggerGroupTask<_TTaskPtr...>::disableLevel(std::size_t level) noexcept
    {
            std::apply([level](auto&... task) { (task->disableLevel(level), ...); }, _task);
            return *this;
    }

    template< typename... _TTaskPtr >
    ALoggerGroupTask<_TTaskPtr...>& ALoggerGroupTask<_TTaskPtr...>::addToLog(std::size_t level, const TLogData& data) noexcept
    {
        std::apply([level,data](auto&... task) { (task->addToLog(level, data), ...); }, _task);
        return *this;
    }

    template< typename... _TTaskPtr >
    ALoggerGroupTask<_TTaskPtr...>& ALoggerGroupTask<_TTaskPtr...>::addToLog(std::size_t level, const TLogData& data, std::chrono::system_clock::time_point time) noexcept
    {
        std::apply([level,data,time](auto&... task) { (task->addToLog(level, data, time), ...); }, _task);
        return *this;
    }

}   // namespace ALogger

#endif // _AVN_LOGGER_LOGGER_GROUP_TASK_H
