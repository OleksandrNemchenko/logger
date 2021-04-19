#ifndef _AVN_LOGGER_GROUP_TASK
#define _AVN_LOGGER_GROUP_TASK

#include <tuple>
#include <type_traits>

#include <avn/logger/logger_base.h>

namespace ALogger
{
    template<typename... _TLogger>
    class LoggerGroupTaskHolder
    {
        template<typename... _TLoggerTask>
        class LoggerGroupTask
        {
        public:
            using TLoggerGroupTaskHolder = LoggerGroupTaskHolder<_TLoggerTask...>;
            LoggerGroupTask(TLoggerGroupTaskHolder* ptr) noexcept: _ptr(ptr) {}

            void SetLevels(TLevelsInitList levels) noexcept     { _ptr->SetLevels(levels); }
            void EnableLevels(TLevelsInitList levels) noexcept  { _ptr->EnableLevels(levels); }
            void DisableLevels(TLevelsInitList levels) noexcept { _ptr->DisableLevels(levels); }
            void EnableLevel(TLevel level) noexcept             { _ptr->EnableLevel(level); }
            void DisableLevel(TLevel level) noexcept            { _ptr->DisableLevel(level); }
        
            void Success(bool success = true) noexcept          { _ptr->Success(success); }
            void Fail(bool fail = true) noexcept                { _ptr->Fail(fail); }

        private:
            TLoggerGroupTaskHolder* _ptr;
        };

    public:
        using TArrayPtr = std::tuple<_TLogger...>;
        using TLoggerGroupTask = LoggerGroupTask<_TLogger...>;

        using TLogData = typename std::tuple_element_t<0, TArrayPtr>::element_type::TLogData;

        static_assert((std::is_same_v<TLogData, typename _TLogger::element_type::TLogData> && ...), "All loggers in the logger group must have the same TLogData type");

        LoggerGroupTaskHolder(TArrayPtr&& tasks) noexcept;
        virtual ~LoggerGroupTaskHolder() = default;

        TLoggerGroupTask* operator->() noexcept             { return &_tasksInterface; }
        const TLoggerGroupTask* operator->() const noexcept { return &_tasksInterface; }

        constexpr auto SizeOf() const noexcept              { return sizeof...(_TLogger); }

        void SetLevels(TLevelsInitList levels) noexcept;
        void EnableLevels(TLevelsInitList levels) noexcept;
        void DisableLevels(TLevelsInitList levels) noexcept;
        void EnableLevel(TLevel level) noexcept;
        void DisableLevel(TLevel level) noexcept;
        
        void Success(bool success = true) noexcept;
        void Fail(bool fail = true) noexcept;

    private:
        TArrayPtr _tasksPtr;
        TLoggerGroupTask _tasksInterface;
    };
}

template<typename... _TLogger>
ALogger::LoggerGroupTaskHolder<_TLogger...>::LoggerGroupTaskHolder(TArrayPtr&& tasks) noexcept :
    _tasksPtr(std::move(tasks)), _tasksInterface(this)
{
}

template<typename... _TLogger>
void ALogger::LoggerGroupTaskHolder<_TLogger...>::SetLevels(TLevelsInitList levels) noexcept
{
    std::apply([&levels] (auto&... task)
    { 
        (task->SetLevels(levels), ...);
    }, _tasksPtr);
}

template<typename... _TLogger>
void ALogger::LoggerGroupTaskHolder<_TLogger...>::EnableLevels(TLevelsInitList levels) noexcept
{
    std::apply([&levels] (auto&... task)
    {
        (task->EnableLevels(levels), ...);
    }, _tasksPtr);
}

template<typename... _TLogger>
void ALogger::LoggerGroupTaskHolder<_TLogger...>::DisableLevels(TLevelsInitList levels) noexcept
{
    std::apply([&levels] (auto&... task)
    {
        (task->DisableLevels(levels), ...);
    }, _tasksPtr);
}

template<typename... _TLogger>
void ALogger::LoggerGroupTaskHolder<_TLogger...>::EnableLevel(TLevel level) noexcept
{
    std::apply([level] (auto&... task)
    {
        (task->EnableLevel(level), ...);
    }, _tasksPtr);
}

template<typename... _TLogger>
void ALogger::LoggerGroupTaskHolder<_TLogger...>::DisableLevel(TLevel level) noexcept
{
    std::apply([level] (auto&... task)
    {
        (task->DisableLevel(level), ...);
    }, _tasksPtr);
}

template<typename... _TLogger>
void ALogger::LoggerGroupTaskHolder<_TLogger...>::Success(bool success) noexcept
{
    std::apply([success] (auto&... task)
    {
        (task->Success(success), ...);
    }, _tasksPtr);
}

template<typename... _TLogger>
void ALogger::LoggerGroupTaskHolder<_TLogger...>::Fail(bool fail) noexcept
{
    std::apply([fail] (auto&... task)
    {
        (task->Fail(fail), ...);
    }, _tasksPtr);
}

#endif // _AVN_LOGGER_GROUP_TASK
