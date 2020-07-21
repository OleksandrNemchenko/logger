#ifndef _AVN_LOGGER_GROUP
#define _AVN_LOGGER_GROUP

#include <tuple>
#include <type_traits>

#include <avn/logger/logger_group_task.h>

namespace ALogger
{
    template<typename... _TLogger>
    class LoggerGroup
    {
    public:
        using TArray = std::tuple<_TLogger...>;
        using TLogData = typename std::tuple_element_t<0, TArray>::TLogData;

        static_assert((std::is_same_v<TLogData, typename _TLogger::TLogData> && ...), "All loggers in the logger group must have the same TLogData type");

        virtual ~LoggerGroup() = default;

        template<size_t num>
        auto& Logger() noexcept;

        template<size_t num>
        const auto& Logger() const noexcept;

        constexpr auto SizeOf() const noexcept  { return sizeof...(_TLogger); }

        void SetLevels(TLevelsInitList levels) noexcept;
        void EnableLevels(TLevelsInitList levels) noexcept;
        void DisableLevels(TLevelsInitList levels) noexcept;
        void EnableLevel(TLevel level) noexcept;
        void DisableLevel(TLevel level) noexcept;
        bool ForceOutput(bool force = true) noexcept;
        
        auto StartTask(bool initialSuccessState = false) noexcept;

        void EnableTasks(bool enable = true) noexcept;
        void DisableTasks(bool disable = true) noexcept;

    protected:
        TArray _loggers;
    };
}

template<typename... _TLogger>
template<size_t num>
auto& ALogger::LoggerGroup<_TLogger...>::Logger() noexcept
{
    static_assert(num < sizeof...(_TLogger), "Requested element's number is out of this logger group size");
    return std::get<num>(_loggers);
}

template<typename... _TLogger>
template<size_t num>
const auto& ALogger::LoggerGroup<_TLogger...>::Logger() const noexcept
{
    static_assert(num < sizeof...(_TLogger), "Requested element's number is out of this logger group size");
    return std::get<num>(_loggers);
}

template<typename... _TLogger>
void ALogger::LoggerGroup<_TLogger...>::SetLevels(TLevelsInitList levels) noexcept
{
    std::apply([&levels] (auto&... logger)
    { 
        (logger.SetLevels(levels), ...);
    }, _loggers);
}

template<typename... _TLogger>
void ALogger::LoggerGroup<_TLogger...>::EnableLevels(TLevelsInitList levels) noexcept
{
    std::apply([&levels] (auto&... logger)
    {
        (logger.EnableLevels(levels), ...);
    }, _loggers);
}

template<typename... _TLogger>
void ALogger::LoggerGroup<_TLogger...>::DisableLevels(TLevelsInitList levels) noexcept
{
    std::apply([&levels] (auto&... logger)
    {
        (logger.DisableLevels(levels), ...);
    }, _loggers);
}

template<typename... _TLogger>
void ALogger::LoggerGroup<_TLogger...>::EnableLevel(TLevel level) noexcept
{
    std::apply([level] (auto&... logger)
    {
        (logger.EnableLevel(level), ...);
    }, _loggers);
}

template<typename... _TLogger>
void ALogger::LoggerGroup<_TLogger...>::DisableLevel(TLevel level) noexcept
{
    std::apply([level] (auto&... logger)
    {
        (logger.DisableLevel(level), ...);
    }, _loggers);
}

template<typename... _TLogger>
bool ALogger::LoggerGroup<_TLogger...>::ForceOutput(bool force) noexcept
{
    bool res = true;
    std::apply([&res, force] (auto&... logger)
    {
        res &= (logger.ForceOutput(force), ...);
    }, _loggers);

    return res;
}

template<typename... _TLogger>
auto ALogger::LoggerGroup<_TLogger...>::StartTask(bool initialSuccessState)  noexcept
{
    return LoggerGroupTaskHolder(std::apply([initialSuccessState](auto&... logger)
        {
            return std::make_tuple(logger.StartTask(initialSuccessState) ...);
        }, _loggers));
}

template<typename... _TLogger>
void ALogger::LoggerGroup<_TLogger...>::EnableTasks(bool enable) noexcept
{
    std::apply([enable] (auto&... logger) {
        (logger.EnableTasks(enable), ...);
    }, _loggers);
}

template<typename... _TLogger>
void ALogger::LoggerGroup<_TLogger...>::DisableTasks(bool disable) noexcept
{
    std::apply([disable] (auto&... logger) {
        (logger.DisableTasks(disable), ...);
    }, _loggers);
}

#endif // _AVN_LOGGER_GROUP
