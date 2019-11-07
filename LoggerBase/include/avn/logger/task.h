// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef _AVN_LOGGER_BASE_TASK_H_
#define _AVN_LOGGER_BASE_TASK_H_

#include <chrono>
#include <vector>

#include <avn/logger/data_types.h>

namespace Logger {

    template<typename TLogData> class CTask;

    template<typename TLogData>
    class CLoggerTaskInterface{
        friend class CTask<TLogData>;

    protected:
        virtual bool ForceAddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time ) = 0;
        virtual const TLevels& GetLevels() const = 0;

        CTask<TLogData> CreateTask( bool init_succeeded ) {
            return CTask<TLogData>( *this, init_succeeded );
        };

    private:
        virtual void RemoveTask() = 0;
    };

    template<typename TLogData>
    class CTask {
        friend class CLoggerTaskInterface<TLogData>;

    private:
        CTask( CLoggerTaskInterface<TLogData> &logger, bool init_succeeded = false ) :
        _succeeded( init_succeeded ), _logger( logger ), _out_levels( logger.GetLevels() )
            {}

    public:
        CTask( const CTask & ) = delete;
        CTask( CTask && ) = default;
        ~CTask();

        CTask& SetTaskResult( bool succeeded )  { _succeeded = succeeded; return *this; }
        CTask& SetSuccess()                     { return SetTaskResult( true ); }
        CTask& SetFail()                        { return SetTaskResult( false ); }
        bool TaskResult() const                 { return _succeeded; }

        CTask& AddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time );

        CTask& InitLevel( std::size_t level, bool to_output );
        CTask& SetLevels( TLevels levels )    { _out_levels = levels; return *this; }
        CTask& OnLevel( std::size_t level )   { InitLevel(level, true);  return *this; }
        CTask& OffLevel( std::size_t level )  { InitLevel(level, false); return *this; }

    private:
        struct SLogEntry {
            SLogEntry( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time ) :
                _time(time), _level(level), _data(std::forward<TLogData>(data)) {}

            std::chrono::system_clock::time_point _time;
            std::size_t _level;
            TLogData _data;
        };

        CLoggerTaskInterface<TLogData>& _logger;
        TLevels _out_levels;
        std::vector<SLogEntry> _log_entries;
        bool _succeeded;
    };

    template<typename TLogData>
    CTask<TLogData>& CTask<TLogData>::InitLevel( std::size_t level, bool to_output ) {
        if( to_output ) _out_levels.emplace( level );
        else            _out_levels.erase( level );
        return *this;
    }

    template<typename TLogData>
    CTask<TLogData>& CTask<TLogData>::AddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time ) {
        _log_entries.emplace_back( level, std::forward<TLogData>(data), time );
        return *this;
    }

    template<typename TLogData>
    CTask<TLogData>::~CTask() {
        for( auto &entry : _log_entries ) {
            if( !_succeeded || _out_levels.count( entry._level ))
                _logger.ForceAddToLog( entry._level, std::move( entry._data ), entry._time );
        }

        _log_entries.clear();
        _logger.RemoveTask();
    }

} // namespace Logger

#endif  // _AVN_LOGGER_BASE_TASK_H_
