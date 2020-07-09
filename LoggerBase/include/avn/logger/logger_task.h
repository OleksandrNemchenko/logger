#ifndef _AVN_LOGGER_TASK
#define _AVN_LOGGER_TASK

#include <memory>
#include <set>
#include <vector>

namespace ALogger
{
    template<typename _TLogData>
    class LoggerBase;

    template<typename _TLogData>
    class LoggerTask
    {
        friend class LoggerBase<_TLogData>;

    public:
        using TLogger = LoggerBase<_TLogData>;
        using TLogData = _TLogData;
        using TLevel = size_t;
        using TLevels = std::set<TLevel>;
        using TLevelsInitList = std::initializer_list<TLevels::value_type>;

        virtual ~LoggerTask();

        const TLevels& Levels() const noexcept      { return _levels; }

        void SetLevels(TLevelsInitList levels) noexcept     { _levels = levels; }
        void EnableLevels(TLevelsInitList levels) noexcept  { for (const auto level : levels) EnableLevel(level); }
        void DisableLevels(TLevelsInitList levels) noexcept { for (const auto level : levels) DisableLevel(level); }
        void EnableLevel(TLevel level) noexcept     { _levels.emplace(level); }
        void DisableLevel(TLevel level) noexcept    { _levels.erase(level); }

        void Success(bool success = true) noexcept  { _success = success; }
        void Fail(bool fail = true) noexcept        { _success = !fail; }

        LoggerTask(bool initialSuccessState, TLogger* logger);

    private:
        struct SLogEntry
        {
            TLevel _level;
            TLogData _data;
            template<typename TData>
            SLogEntry(TLevel level, TData&& data): _level(level), _data(std::forward<TData>(data)) {}
        };

        bool _success;
        TLogger& _logger;
        TLevels _levels;
        std::vector<SLogEntry> _logEntries;


        template<typename TData>
        void AddToLog(TLevel level, TData&& logData) noexcept   { _logEntries.emplace_back(level, std::forward<TData>(logData)); }
    };
}

template<typename _TLogData>
ALogger::LoggerTask<_TLogData>::LoggerTask(bool initialSuccessState, TLogger* logger):
    _logger(*logger), _success(initialSuccessState)
{
    assert(logger);
    _levels = _logger.Levels();
}

template<typename _TLogData>
ALogger::LoggerTask<_TLogData>::~LoggerTask()
{
    for (const auto& logEntry : _logEntries)
    {
        if (!_success || _levels.count(logEntry._level))
            _logger.Output(logEntry._data);
    }
}

#endif // _AVN_LOGGER_TASK
