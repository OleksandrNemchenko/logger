// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef _AVN_LOGGER_BASE_TASK_H_
#define _AVN_LOGGER_BASE_TASK_H_

#include <chrono>
#include <vector>

#include <avn/logger/data_types.h>

namespace Logger {

    template<typename _TLogData> class CTask;
    template<typename _TLogData> class ILoggerGroup;

    template<typename _TLogData>
    class ITaskLogger{
        friend class CTask<_TLogData>;

    protected:
        virtual const TLevels& Levels() const = 0;
        virtual bool ForceAddToLog( std::size_t level, _TLogData &&data, std::chrono::system_clock::time_point time ) = 0;
        virtual void RemoveTask() = 0;

        CTask<_TLogData> CreateTask( bool init_success_state ) {
            return CTask<_TLogData>( *this, init_success_state );
        };
    };

    template< typename _TLogData >
    class CTask {
        friend class ITaskLogger<_TLogData>;
        friend class ILoggerGroup<_TLogData>;

    private:
        CTask( ITaskLogger<_TLogData> &logger, bool init_succeeded ) :
                _success_state( init_succeeded ), _logger( logger ), _out_levels( logger.Levels() )
            {}

    public:
        CTask( const CTask & ) = delete;
        CTask( CTask && ) = default;
        ~CTask();

        CTask& SetTaskResult( bool init_success_state ) { _success_state = init_success_state; return *this; }
        CTask& SetSuccess()                             { return SetTaskResult( true ); }
        CTask& SetFail()                                { return SetTaskResult( false ); }
        bool TaskResult() const                         { return _success_state; }

        CTask& AddToLog( std::size_t level, _TLogData &&data, std::chrono::system_clock::time_point time );

        CTask& InitLevel( std::size_t level, bool to_output );
        CTask& SetLevels( TLevels levels )    { _out_levels = levels; return *this; }
        CTask& OnLevel( std::size_t level )   { InitLevel(level, true);  return *this; }
        CTask& OffLevel( std::size_t level )  { InitLevel(level, false); return *this; }

    private:
        struct SLogEntry {
            SLogEntry( std::size_t level, _TLogData &&data, std::chrono::system_clock::time_point time ) :
                _time(time), _level(level), _data(std::forward<_TLogData>(data)) {}

            std::chrono::system_clock::time_point _time;
            std::size_t _level;
            _TLogData _data;
        };

        ITaskLogger<_TLogData>& _logger;
        TLevels _out_levels;
        std::vector<SLogEntry> _log_entries;
        bool _success_state;
    };

    template<typename _TLogData>
    CTask<_TLogData>& CTask<_TLogData>::InitLevel( std::size_t level, bool to_output ) {
        if( to_output ) _out_levels.emplace( level );
        else            _out_levels.erase( level );
        return *this;
    }

    template<typename _TLogData>
    CTask<_TLogData>& CTask<_TLogData>::AddToLog( std::size_t level, _TLogData &&data, std::chrono::system_clock::time_point time ) {
        _log_entries.emplace_back( level, std::forward<_TLogData>(data), time );
        return *this;
    }

    template<typename _TLogData>
    CTask<_TLogData>::~CTask() {
        for( auto &entry : _log_entries ) {
            if( !_success_state || _out_levels.count( entry._level ))
                _logger.ForceAddToLog( entry._level, std::move( entry._data ), entry._time );
        }

        _log_entries.clear();
        _logger.RemoveTask();
    }

} // namespace Logger

#endif  // _AVN_LOGGER_BASE_TASK_H_
