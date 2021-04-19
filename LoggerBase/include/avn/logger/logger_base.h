#ifndef _AVN_LOGGER_BASE
#define _AVN_LOGGER_BASE

#include <cassert>
#include <deque>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <thread>

#include <avn/logger/logger_task.h>

namespace ALogger
{
    using TLevel = size_t;
    using TLevels = std::set<TLevel>;
    using TLevelsInitList = std::initializer_list<TLevels::value_type>;

    template<typename _TLogData>
    class LoggerBase
    {
        friend class LoggerTask<_TLogData>;

    public:
        using TLogData = _TLogData;
        using TTask = LoggerTask<_TLogData>;
        using TTasks = std::deque<TTask>;
        using TThreads = std::map<std::thread::id, TTasks>;

        LoggerBase(bool threadSafe = true) noexcept;
        virtual ~LoggerBase();

        const TLevels& Levels() const noexcept              { return _levels; }
        const TThreads& Threads() const noexcept            { return _threads; }
        const TTasks& Tasks() const noexcept                { return _threads.at(std::this_thread::get_id()); }

        bool IsLoggerEnabled() const noexcept               { return _loggerEnabled;  }
        bool IsThreadSafe() const noexcept                  { return _threadSafe; }

        void EnableLogger() noexcept                        { _loggerEnabled = true; }
        void DisableLogger() noexcept                       { _loggerEnabled = false; }
        void SetThreadSafety(bool threadSafe) noexcept;

        void SetLevels(TLevelsInitList levels) noexcept     { _levels = levels; }
        void EnableLevels(TLevelsInitList levels) noexcept  { for (const auto level : levels) EnableLevel(level); }
        void DisableLevels(TLevelsInitList levels) noexcept { for (const auto level : levels) DisableLevel(level); }
        void EnableLevel(TLevel level) noexcept             { _levels.emplace(level); }
        void DisableLevel(TLevel level) noexcept            { _levels.erase(level); }
        bool ForceOutput(bool force = true) noexcept        { bool previousForceState = _forceOutput; _forceOutput = force; return previousForceState; }

        auto StartTask(bool initialSuccessState = false) noexcept;
        void EnableTasks(bool enable = true) noexcept       { _enableTasks = enable; }
        void DisableTasks(bool disable = true) noexcept     { _enableTasks = !disable; }

    protected:
        template<typename TData>
        bool AddToLog(TLevel level, TData&& logData) noexcept;

        bool CanBeAddedToLog(TLevel level) noexcept;

        virtual bool OutputImpl(const TLogData& logData) noexcept = 0;
        bool Output(const TLogData& logData) noexcept;

    private:
        bool _threadSafe;
        bool _forceOutput = false;
        bool _enableTasks = true;
        bool _loggerEnabled = true;
        TLevels _levels;
        TThreads _threads;

        std::unique_ptr<std::recursive_mutex> _mutex;

        TTask* AddTaskSafely(bool initialSuccessState) noexcept;

        void RemoveTask(TTask* task) noexcept;
        void RemoveTaskSafely(TTask* task) noexcept;

        TTask* GetTask() noexcept;
        TTask* GetTaskSafely() noexcept;
    };
}

template<typename _TLogData>
ALogger::LoggerBase<_TLogData>::LoggerBase(bool threadSafe) noexcept :
    _threadSafe(false)
{
    SetThreadSafety(threadSafe);
}

template<typename _TLogData>
ALogger::LoggerBase<_TLogData>::~LoggerBase()
{
    for (const auto& thread : _threads)
    {
        (void) thread;
        assert(thread.second.empty());
    }
}

template<typename _TLogData>
void ALogger::LoggerBase<_TLogData>::SetThreadSafety(bool threadSafe) noexcept
{
    if (_threadSafe == threadSafe)
        return;

    _threadSafe = threadSafe;

    if (!_threadSafe)
        _mutex.reset();
    else
        _mutex = std::make_unique<std::recursive_mutex>();
}

template<typename _TLogData>
template<typename TData>
bool ALogger::LoggerBase<_TLogData>::AddToLog(TLevel level, TData&& logData) noexcept
{
    if (!_loggerEnabled)
        return false;

    if (_forceOutput)
        return Output(logData);

    if (_enableTasks)
    {
        auto task = GetTask();
        if (task)
        {
            task->AddToLog(level, std::forward<TData>(logData));
            return true;
        }
    }

    if (_levels.count(level))
        return Output(logData);
    else
        return false;
}

template<typename _TLogData>
bool ALogger::LoggerBase<_TLogData>::CanBeAddedToLog(TLevel level) noexcept
{
    if (!_loggerEnabled)
        return false;

    if (_forceOutput)
        return true;

    if (_enableTasks)
    {
        auto task = GetTask();
        if (task)
        {
            return true;
        }
    }

    return _levels.count(level) != 0;
}

template<typename _TLogData>
bool ALogger::LoggerBase<_TLogData>::Output(const TLogData& logData) noexcept
{
    if (_threadSafe)
    {
        assert(_mutex);
        std::lock_guard<std::recursive_mutex> guard(*_mutex.get());
        return OutputImpl(logData);
    }
    else
    {
        return OutputImpl(logData);
    }
}

template<typename _TLogData>
auto ALogger::LoggerBase<_TLogData>::StartTask(bool initialSuccessState) noexcept
{
    TTask* newTask = nullptr;

    if (_threadSafe)
    {
        assert(_mutex);
        std::lock_guard<std::recursive_mutex> guard(*_mutex.get());
        newTask = AddTaskSafely(initialSuccessState);
    }
    else
    {
        newTask = AddTaskSafely(initialSuccessState);
    }

    assert(newTask);
    
    auto taskDeleter = [](TTask* task){  task->_logger.RemoveTask(task); };
    std::unique_ptr<TTask, decltype(taskDeleter)> newTaskUPtr(newTask);

    return std::move(newTaskUPtr);
}

template<typename _TLogData>
typename ALogger::LoggerBase<_TLogData>::TTask* ALogger::LoggerBase<_TLogData>::AddTaskSafely(bool initialSuccessState) noexcept
{
    auto &currentThreadTasks = _threads[std::this_thread::get_id()];
    currentThreadTasks.emplace_back(initialSuccessState, this);

    return &currentThreadTasks.back();
}

template<typename _TLogData>
void ALogger::LoggerBase<_TLogData>::RemoveTask(TTask* task) noexcept
{
    if (_threadSafe)
    {
        assert(_mutex);
        std::lock_guard<std::recursive_mutex> guard(*_mutex.get());
        RemoveTaskSafely(task);
    }
    else
    {
        RemoveTaskSafely(task);
    }
}

template<typename _TLogData>
void ALogger::LoggerBase<_TLogData>::RemoveTaskSafely(TTask* task) noexcept
{
    (void) task;
    assert(task);

    auto thisThreadId = std::this_thread::get_id();
    assert(!_threads[thisThreadId].empty());

    auto& back = _threads[thisThreadId].back();
    (void) back;
    assert(&back == task);

    _threads[thisThreadId].pop_back();
}

template<typename _TLogData>
ALogger::LoggerTask<_TLogData>* ALogger::LoggerBase<_TLogData>::GetTask() noexcept
{
    if (_threadSafe)
    {
        assert(_mutex);
        std::lock_guard<std::recursive_mutex> guard(*_mutex.get());
        return GetTaskSafely();
    }
    else
    {
        return GetTaskSafely();
    }
}

template<typename _TLogData>
ALogger::LoggerTask<_TLogData>* ALogger::LoggerBase<_TLogData>::GetTaskSafely() noexcept
{
    auto thisThreadId = std::this_thread::get_id();
    auto threadTasks = std::find_if(_threads.begin(), _threads.end(), [thisThreadId](const auto& thread)
    {
        return thread.first == thisThreadId;
    });

    if (threadTasks == _threads.cend())
        return nullptr;

    auto& tasks = threadTasks->second;
    if (tasks.empty())
        return nullptr;

    return &tasks.back();
}

#endif // _AVN_LOGGER_BASE
