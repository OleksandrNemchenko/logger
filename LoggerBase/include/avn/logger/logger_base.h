
#pragma once

#include <chrono>
#include <set>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Logger {

    template<typename TLogData>
    class CLoggerBase {

    public:
        using TLevels = std::set<std::size_t>;

    public:
        class CTask {
        public:
            CTask(CLoggerBase &logger) : _logger(logger), _out_levels(logger.GetLevels()) {}

            CTask(const CTask &) = delete;

            ~CTask() { Flush(true); }

            void Flush(bool success);

            void AddToLog(std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time);

            void InitLevel(std::size_t level, bool to_output);
            void SetLevels(TLevels levels) { _out_levels = levels; }
            void OnLevel(std::size_t level) { InitLevel(level, true); }
            void OffLevel(std::size_t level) { InitLevel(level, false); }

        private:
            struct SLogEntry {
                SLogEntry(std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time)
                        : _time(), _level(level), _data(std::forward<TLogData>(data)) {}

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

        CLoggerBase(const CLoggerBase &) = delete;

    protected:
        const TLevels &GetLevels() const { return _out_levels; }

        const TTasks &GetTasks() const { return _tasks; }

        CTask &AddTask(void);

        CTask &AddTask(TLevels levels);

        void FinishTask(bool success);

        void InitLevel(std::size_t level, bool to_output);
        void OnLevel(std::size_t level)  { InitLevel(level, true); }
        void OffLevel(std::size_t level) { InitLevel(level, false); }
        void SetLevels(TLevels levels)   { _out_levels = levels; }

        bool ForceAddToLog(std::size_t level, TLogData &&data,
                           std::chrono::system_clock::time_point time = std::chrono::system_clock::now());

        bool AddToLog(std::size_t level, TLogData &&data,
                      std::chrono::system_clock::time_point time = std::chrono::system_clock::now());

        virtual bool OutStrings(std::size_t level, std::chrono::system_clock::time_point time, TLogData &&data) = 0;

    private:
        TLevels _out_levels;
        std::unordered_map<std::thread::id, CTask> _tasks;

    };

    template<typename TLogData>
    void CLoggerBase<TLogData>::CTask::InitLevel(std::size_t level, bool to_output) {
        if (to_output)
            _out_levels.emplace(level);
        else
            _out_levels.erase(level);
    }

    template<typename TLogData>
    void
    CLoggerBase<TLogData>::CTask::AddToLog(std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time) {
        _log_entries.emplace_back(level, std::forward<TLogData>(data), time);
    }

    template<typename TLogData>
    void CLoggerBase<TLogData>::CTask::Flush(bool success) {
        for (auto &entry : _log_entries) {
            if (!success || _out_levels.count(entry._level))
                _logger.ForceAddToLog(entry._level, std::move(entry._data), entry._time);
        }

        _log_entries.clear();
    }

    template<typename TLogData>
    typename CLoggerBase<TLogData>::CTask &CLoggerBase<TLogData>::AddTask(void) {
        auto[it, inserted] = _tasks.emplace(std::this_thread::get_id(), *this);
        (void) inserted;
        CTask &task = it->second;
        return task;
    }

    template<typename TLogData>
    typename CLoggerBase<TLogData>::CTask &CLoggerBase<TLogData>::AddTask(TLevels levels) {
        auto &task = AddTask();
        task.SetLevels(std::forward<TLevels>(levels));
        return task;
    }

    template<typename TLogData>
    void CLoggerBase<TLogData>::FinishTask(bool success) {
        if (auto task = _tasks.find(std::this_thread::get_id()); task != _tasks.end()) {
            task->second.Flush(success);
            _tasks.erase(task);
        }
    }

    template<typename TLogData>
    void CLoggerBase<TLogData>::InitLevel(std::size_t level, bool to_output) {
        if (to_output)  _out_levels.emplace(level);
        else            _out_levels.erase(level);
    }

    template<typename TLogData>
    bool CLoggerBase<TLogData>::AddToLog(std::size_t level, TLogData &&data,
                                     std::chrono::system_clock::time_point time /* = std::chrono::system_clock::now() */ ) {
        if (auto task = _tasks.find(std::this_thread::get_id()); task != _tasks.end()) {
            task->second.AddToLog(level, std::forward<TLogData>(data), time);
            return true;
        } else {
            if (_out_levels.count(level) > 0)
                return OutStrings(level, time, std::forward<TLogData>(data));
            else
                return false;
        }
    }

    template<typename TLogData>
    bool CLoggerBase<TLogData>::ForceAddToLog(std::size_t level, TLogData &&data,
                                          std::chrono::system_clock::time_point time /* = std::chrono::system_clock::now() */ ) {
        return OutStrings(level, time, std::forward<TLogData>(data));
    }

} // namespace Logger