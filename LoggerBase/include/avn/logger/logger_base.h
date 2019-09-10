
#pragma once

#include <chrono>
#include <set>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

template<typename TStr>
class CLoggerBase{

public:
    using TLogItem = std::pair<TStr, TStr>;
    using TLogData = std::vector<TLogItem>;
    using TLevels = std::set<std::size_t>;

public:
    class CTask{
    public:
        CTask(CLoggerBase &logger): _logger(logger), _out_levels(logger.GetLevels()) { }
        CTask(const CTask&) = delete;
        ~CTask()                            { Flush(true); }

        void Flush(bool success);
        void AddToLog(std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time);

        void InitLevel(std::size_t level, bool to_output);
        void AddLevel(std::size_t level)    { InitLevel(level, true); }
        void SetLevels(TLevels levels)      { _out_levels = levels; }
        void RemoveLevel(std::size_t level) { InitLevel(level, false); }

    private:
        struct SLogEntry{
        SLogEntry(std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time): _time(), _level(level), _data(std::forward<TLogData>(data))  {}

        std::chrono::system_clock::time_point _time;
        std::size_t _level;
        TLogData _data;
    };

    CLoggerBase &_logger;
    TLevels _out_levels;
    std::vector<SLogEntry> _log_entries;
};

public:
    using TTasks = std::unordered_map<std::thread::id, CTask>;

    CLoggerBase(void) = default;
    CLoggerBase(const CLoggerBase&) = delete;

protected:
    const TLevels &GetLevels() const    { return _out_levels; }
    const TTasks &GetTasks() const      { return _tasks; }

    CTask &AddTask(void);
    CTask &AddTask(TLevels levels);
    void FinishTask(bool success);
    void InitLevel(std::size_t level, bool to_output);
    void AddLevel(std::size_t level)    { InitLevel(level, true); }
    void RemoveLevel(std::size_t level) { InitLevel(level, false); }
    void SetLevels(TLevels levels)      { _out_levels = levels; }
    bool ForceAddToLog(std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now());
    bool AddToLog(std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now());
    
    virtual bool OutStrings(std::size_t level, std::chrono::system_clock::time_point time, TLogData &&data ) = 0;

private:
    TLevels _out_levels;
    std::unordered_map<std::thread::id, CTask> _tasks;

};

template<typename TStr>
void CLoggerBase<TStr>::CTask::InitLevel(std::size_t level, bool to_output){
    if(to_output)
        _out_levels.emplace(level);
    else
        _out_levels.erase(level);
}

template<typename TStr>
void CLoggerBase<TStr>::CTask::AddToLog(std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time) {
    _log_entries.emplace_back (level, std::forward<TLogData>(data), time);
}

template<typename TStr>
void CLoggerBase<TStr>::CTask::Flush(bool success){
    for( auto &entry : _log_entries) {
        if(!success || _out_levels.count(entry._level))
            _logger.ForceAddToLog(entry._level, std::move(entry._data), entry._time);
    }

    _log_entries.clear();
}

template<typename TStr>
typename CLoggerBase<TStr>::CTask &CLoggerBase<TStr>::AddTask(void){
    auto [it, inserted] = _tasks.emplace(std::this_thread::get_id(), *this);
    (void) inserted;
    CTask &task = it->second;
    return task;
}

template<typename TStr>
typename CLoggerBase<TStr>::CTask &CLoggerBase<TStr>::AddTask(TLevels levels){
    auto &task = AddTask();
    task.SetLevels (std::forward<TLevels>(levels));
    return task;
}

template<typename TStr>
void CLoggerBase<TStr>::FinishTask(bool success){
    if(auto task = _tasks.find(std::this_thread::get_id()); task != _tasks.end()) {
        task->second.Flush(success);
        _tasks.erase(task);
    }
}

template<typename TStr>
void CLoggerBase<TStr>::InitLevel(std::size_t level, bool to_output){
    if(to_output)
        _out_levels.emplace(level);
    else
        _out_levels.erase(level);
}

template<typename TStr>
bool CLoggerBase<TStr>::AddToLog(std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time /* = std::chrono::system_clock::now() */ ) {
    if(auto task = _tasks.find(std::this_thread::get_id()); task != _tasks.end()) {
        task->second.AddToLog(level, std::forward<TLogData>(data), time);
        return true;
    } else {
        if(_out_levels.count(level) > 0)
            return OutStrings(level, time, std::forward<TLogData>(data));
        else
            return false;
    }
}

template<typename TStr>
bool CLoggerBase<TStr>::ForceAddToLog(std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time /* = std::chrono::system_clock::now() */ ) {
    return OutStrings(level, time, std::forward<TLogData>(data));
}
