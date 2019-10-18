// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef _AVN_LOGGER_BASE_H_
#define _AVN_LOGGER_BASE_H_

#include <chrono>
#include <set>
#include <thread>
#include <unordered_map>
#include <vector>

namespace Logger {

    // TODO : off task, i. e. transparently output messages

    template<bool ThrSafe, typename TLogData>
    class CLoggerBase {

    public:
        using TLevels = std::set<std::size_t>;

    public:
        class CTask {
            friend class CLoggerBase;
        private:
            CTask( CLoggerBase &logger, bool init_succeeded = false ) : _succeeded( init_succeeded ), _logger( logger ), _out_levels( logger.GetLevels() ) {}
        public:
            CTask( const CTask & ) = delete;
            CTask( CTask && ) = default;
            ~CTask();

            CTask& SetTaskResult( bool succeeded )  { _succeeded = succeeded; return *this; }
            bool TaskResult() const                 { return _succeeded; }

            void AddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time );

            void InitLevel( std::size_t level, bool to_output );
            void SetLevels( TLevels levels )    { _out_levels = levels; }
            void OnLevel( std::size_t level )   { InitLevel(level, true); }
            void OffLevel( std::size_t level )  { InitLevel(level, false); }

        private:
            struct SLogEntry {
                SLogEntry( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time ) :
                    _time(), _level(level), _data(std::forward<TLogData>(data)) {}

                std::chrono::system_clock::time_point _time;
                std::size_t _level;
                TLogData _data;
            };

            CLoggerBase& _logger;
            TLevels _out_levels;
            std::vector<SLogEntry> _log_entries;
            bool _succeeded;
        };

    public:
        friend class CTask;

        using TTasks = std::unordered_map<std::thread::id, CTask *>;

        CLoggerBase() = default;
        CLoggerBase( const CLoggerBase & ) = delete;

    protected:
        const TLevels& GetLevels() const { return _out_levels; }

        const TTasks& GetTasks() const { return _tasks; }

        CTask AddTask( bool init_succeeded = false );
        CTask AddTask( TLevels levels, bool init_succeeded = false );

        void InitLevel( std::size_t level, bool to_output );
        void OnLevel( std::size_t level )  { InitLevel(level, true); }
        void OffLevel( std::size_t level ) { InitLevel(level, false); }
        void SetLevels( TLevels levels )   { _out_levels = levels; }
        bool ToBeAdded( std::size_t level ) const;

        bool ForceAddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now() );

        bool AddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now() );

        virtual bool OutStrings( std::size_t level, std::chrono::system_clock::time_point time, TLogData &&data ) = 0;

    private:
        TLevels _out_levels;
        TTasks _tasks;
        std::mutex _out_mutex;

        bool OutStringsThrSafe( std::size_t level, std::chrono::system_clock::time_point time, TLogData &&data );
        void FinishTask();
    };

    template<bool ThrSafe, typename TLogData>
    void CLoggerBase<ThrSafe, TLogData>::CTask::InitLevel( std::size_t level, bool to_output ) {
        if( to_output )
            _out_levels.emplace( level );
        else
            _out_levels.erase( level );
    }

    template<bool ThrSafe, typename TLogData>
    void
    CLoggerBase<ThrSafe, TLogData>::CTask::AddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time ) {
        _log_entries.emplace_back( level, std::forward<TLogData>(data), time );
    }

    template<bool ThrSafe, typename TLogData>
    CLoggerBase<ThrSafe, TLogData>::CTask::~CTask() {
        for( auto &entry : _log_entries ) {
            if( !_succeeded || _out_levels.count( entry._level ))
                _logger.ForceAddToLog( entry._level, std::move( entry._data ), entry._time );
        }

        _log_entries.clear();
        _logger.FinishTask();
    }

    template<bool ThrSafe, typename TLogData>
    typename CLoggerBase<ThrSafe, TLogData>::CTask CLoggerBase<ThrSafe, TLogData>::AddTask( bool init_succeeded ) {
        CTask task( *this, init_succeeded );
        auto[ it, inserted ] = _tasks.emplace( std::this_thread::get_id(), &task );
        (void) inserted;
        return task;
    }

    template<bool ThrSafe, typename TLogData>
    typename CLoggerBase<ThrSafe, TLogData>::CTask CLoggerBase<ThrSafe, TLogData>::AddTask(TLevels levels, bool init_succeeded ) {
        auto task = AddTask( init_succeeded );
        task.SetLevels( std::forward<TLevels>(levels) );
        return task;
    }

    template<bool ThrSafe, typename TLogData>
    void CLoggerBase<ThrSafe, TLogData>::FinishTask() {
        if( auto task = _tasks.find( std::this_thread::get_id() ); task != _tasks.end() ) {
            _tasks.erase( task );
        } else {
            assert( false );
        }
    }

    template<bool ThrSafe, typename TLogData>
    void CLoggerBase<ThrSafe, TLogData>::InitLevel( std::size_t level, bool to_output ) {
        if( to_output ) _out_levels.emplace( level );
        else            _out_levels.erase( level );
    }

    template<bool ThrSafe, typename TLogData>
    bool CLoggerBase<ThrSafe, TLogData>::ToBeAdded( std::size_t level ) const {
        if( auto task = _tasks.find(std::this_thread::get_id()); task != _tasks.end() ) {
            return true;
        } else {
            if ( _out_levels.count(level) )
                return true;
            else
                return false;
        }
    }

    template<bool ThrSafe, typename TLogData>
    bool CLoggerBase<ThrSafe, TLogData>::AddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time ) {
        if( auto task = _tasks.find( std::this_thread::get_id() ); task != _tasks.end() ) {
            task->second->AddToLog( level, std::forward<TLogData>(data), time );
            return true;
        } else if( ToBeAdded(level) )
            return OutStringsThrSafe( level, time, std::forward<TLogData>(data) );
        else
            return false;
    }

    template<bool ThrSafe, typename TLogData>
    bool CLoggerBase<ThrSafe, TLogData>::ForceAddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time ) {
        return OutStringsThrSafe( level, time, std::forward<TLogData>(data) );
    }

    template<bool ThrSafe, typename TLogData>
    bool CLoggerBase<ThrSafe, TLogData>::OutStringsThrSafe( std::size_t level, std::chrono::system_clock::time_point time, TLogData &&data ) {
        class Safe{
        public:
            Safe( std::mutex &mutex ) : _mutex (mutex){ if constexpr( ThrSafe ) _mutex.lock(); }
            ~Safe() {                                   if constexpr( ThrSafe ) _mutex.unlock(); }
        private:
            std::mutex &_mutex;
        };

        Safe safe_lock( _out_mutex );

        return OutStrings( level, time, std::move( data ));
    }

} // namespace Logger

#endif  // _AVN_LOGGER_BASE_H_
