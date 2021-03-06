//
// Created by Oleksandr Nemchenko on 11/5/19.
//

/*! \file logger_group.h
 * \brief ALoggerGroup class implements loggers grouping.
 *
 * #ALogger::ALoggerGroup class is the templated container for different loggers that can be used simultaneously, i. e. you output
 * the same logger message to different targets. For example, you can output the same message for console, disk, network etc.
 * You can enable different logger levels for each target.
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

_log.enableLevel(WARNING);                            // Enables WARNING levels
_log.addString(WARNING, L"Warning message!");         // Outputs message

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

namespace ALogger {

    /** Interface for internal usage */
    template< typename _TLogData >
    class ILoggerGroup {
        template< typename... _TLogger >
        friend class ALoggerGroup;

    protected:
        ALoggerTask<_TLogData>* createTask(ITaskLogger<_TLogData>& logger, bool init_success_state) noexcept
        {
            return new ALoggerTask<_TLogData>(logger, init_success_state);
        };

    private:
        virtual ALoggerTask<_TLogData>* addTaskForLoggerGroup(bool init_succeeded) noexcept = 0;
        virtual ALoggerTask<_TLogData>* addTaskForLoggerGroup() noexcept = 0;
        virtual ALoggerTask<_TLogData>* addTaskForLoggerGroup(TLevels levels, bool init_succeeded) noexcept = 0;
        virtual ALoggerTask<_TLogData>* addTaskForLoggerGroup(TLevels levels) noexcept = 0;

    };  // class ILoggerGroup

    /** Loggers group container
     *
     * This container allows to manage different loggers ans send message by one call.
     *
     * \warning All loggers inside container must have the same \a TLogData logger message data type.
     *
     * \tparam _TLogger Loggers
     */
    template< typename... _TLogger >
    class ALoggerGroup {
    public:
        /** Loggers array type */
        using TArray = std::tuple<_TLogger...>;

        /** ALogger data type */
        using TLogData = typename std::tuple_element_t<0, TArray>::TLogData;

        static_assert((std::is_same_v<TLogData, typename _TLogger::TLogData> && ...), "All loggers in the logger group must have the same TLogData type");

        /** Return logger reference to the \a num element
         *
         * \tparam num Logger number.
         * \warning Must be less than TArray arguments amount
         *
         * \return Specified logger reference
         */
        template< size_t num > auto& logger() noexcept
        {
            static_assert(num < sizeof...(_TLogger), "Requested element's number is out of this logger group size");
            return std::get<num>(_logger);
        }

        /** Return logger reference to the \a num element
         *
         * \tparam num Logger number.
         * \warning Must be less than TArray arguments amount
         *
         * \return Specified logger reference
         */
        template< size_t num > const auto& logger() const noexcept
        {
            static_assert(num < sizeof...(_TLogger), "Requested element's number is out of this logger group size");
            return std::get<num>(_logger);
        }

        /** Return loggers amount
         *
         * \return Loggers amount.
         */
        constexpr auto sizeOf() const noexcept
        {
            return sizeof...(_TLogger);
        }

        /** Enable or disable specified level for all loggers inside container
         *
         * \param[in] level Level to be enabled or disabled
         * \param[in] to_enable If true, \a level will be enabled. Otherwise it will be disabled
         */
        void initLevel(std::size_t level, bool to_enable) noexcept;

        /** Enable specified level for all loggers inside container
         *
         * \param[in] level Level to be enabled
         */
        void enableLevel(std::size_t level) noexcept;

        /** Disable specified level for all loggers inside container
         *
         * \param[in] level Level to be disabled
         */
        void disableLevel(std::size_t level) noexcept;

        /** Disable tasks
         *
         * This call is useful for debug mode whe you need to see all messages instantly
         */
        void disableTasks() noexcept;

        /** Set levels for all loggers inside container
         *
         * \param[in] levels Levels container
         */
        void setLevels(TLevels levels) noexcept;

        /** Force the message to be output for all loggers inside container
         *
         * Message will be output regardless level and task presence.
         *
         * \param[in] level Message level
         * \param[in] data Message to be output
         * \param[in] time Timestamp. Current time by default
         *
         * \return true if message is output
         */
        bool forceAddToLog(std::size_t level, const TLogData& data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now()) noexcept;

        /** Output the message for all loggers inside container
         *
         * \param[in] level Message level
         * \param[in] data Message to be output
         * \param[in] time Timestamp. Current time by default
         *
         * \return true if message is output
         */
        bool addToLog(std::size_t level, const TLogData& data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now()) noexcept;

        /** Add task for all loggers inside container
         *
         * \return Task object
         */
        auto addTask() noexcept;

        /** Add task with \a init_success_state for all loggers inside container
         *
         * \param[in] init_success_state Initial task state
         *
         * \return Task object
         */
        auto addTask(bool init_success_state) noexcept;

        /** Add task with \a levels enabled for all loggers inside container
         *
         * \param[in] levels Levels container
         *
         * \return Task object
         */
        auto addTask(TLevels levels) noexcept;

        /** Add task with \a init_success_state and \a levels enabled for all loggers inside container
         *
         * \param[in] levels Levels container
         * \param[in] init_success_state Initial task state
         *
         * \return Tasks object
         */
        auto addTask(TLevels levels, bool init_success_state) noexcept;

    protected:
        TArray _logger;

    };  // class ALoggerGroup

    template< typename... _TLogger >
    void ALoggerGroup<_TLogger...>::initLevel(std::size_t level, bool to_enable) noexcept {
        std::apply([=] (auto&... logger) {
            (logger.initLevel(level, to_enable), ...);
        }, _logger);
    }

    template< typename... _TLogger >
    void ALoggerGroup<_TLogger...>::enableLevel(std::size_t level) noexcept {
        std::apply([=](auto&... logger) {
            (logger.enableLevel(level), ...);
        }, _logger);
    }

    template< typename... _TLogger >
    void ALoggerGroup<_TLogger...>::disableLevel(std::size_t level) noexcept {
        std::apply([=](auto&... logger) {
            (logger.disableLevel(level), ...);
        }, _logger);
    }

    template< typename... _TLogger >
    void ALoggerGroup<_TLogger...>::setLevels(TLevels levels) noexcept {
        std::apply([&](auto&... logger) { (logger.setLevels(levels), ...); }, _logger);
    }

    template< typename... _TLogger >
    bool ALoggerGroup<_TLogger...>::forceAddToLog(std::size_t level, const TLogData& data, std::chrono::system_clock::time_point time) noexcept {
        bool res{true};
        std::apply([&](auto&... logger) { (res &= ... &= logger.forceAddToLog(level, data, time)); }, _logger);
        return res;
    }

    template< typename... _TLogger >
    void ALoggerGroup<_TLogger...>::disableTasks() noexcept {
        std::apply([&](auto&... logger) { (logger.disableTasks(), ...); }, _logger);
    }

    template< typename... _TLogger >
    bool ALoggerGroup<_TLogger...>::addToLog(std::size_t level, const TLogData& data, std::chrono::system_clock::time_point time) noexcept {
        bool res{true};
        std::apply([&](auto&... logger) { (res &= ... &= logger.addToLog(level, data, time)); }, _logger);
        return res;
    }

    template< typename... _TLogger >
    auto ALoggerGroup<_TLogger...>::addTask() noexcept {
        auto tasks{ std::apply([](auto&&... logger){
            return std::make_tuple((logger.getLoggerGroupInterface())->addTaskForLoggerGroup() ...);
        }, _logger) };
        return ALoggerGroupTask(std::move(tasks));
    }

    template< typename... _TLogger >
    auto ALoggerGroup<_TLogger...>::addTask(bool init_success_state) noexcept {
        auto tasks{ std::apply([init_success_state](auto&&... logger){
            return std::make_tuple((logger.getLoggerGroupInterface())->addTaskForLoggerGroup(init_success_state) ...);
        }, _logger) };
        return ALoggerGroupTask(std::move(tasks));
    }

    template< typename... _TLogger >
    auto ALoggerGroup<_TLogger...>::addTask(TLevels levels) noexcept {
        auto tasks{ std::apply([&levels](auto&&... logger){
            return std::make_tuple((logger.getLoggerGroupInterface())->addTaskForLoggerGroup(levels) ...);
        }, _logger) };
        return ALoggerGroupTask(std::move(tasks));
    }

    template< typename... _TLogger >
    auto ALoggerGroup<_TLogger...>::addTask(TLevels levels, bool init_success_state) noexcept {
        auto tasks{ std::apply([&levels,init_success_state](auto&&... logger){
            return std::make_tuple((logger.getLoggerGroupInterface())->addTaskForLoggerGroup(levels, init_success_state) ...);
        }, _logger) };
        return ALoggerGroupTask(std::move(tasks));
    }

}   // namespace ALogger

#endif // _AVN_LOGGER_LOGGER_GROUP_H
