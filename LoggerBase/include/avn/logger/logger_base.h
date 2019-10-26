// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef _AVN_LOGGER_BASE_H_
#define _AVN_LOGGER_BASE_H_

#include <chrono>
#include <map>
#include <set>
#include <stack>
#include <thread>
#include <vector>

namespace Logger {

    // TODO : off task, i. e. transparently output messages

    template<bool ThrSafe, typename TLogData> class CLoggerBaseThrSafety;

    template<bool ThrSafe, typename TLogData>
    class CLoggerBase : public CLoggerBaseThrSafety<ThrSafe, TLogData>{

    public:
        using TLevels = std::set<std::size_t>;

    public:
        class CTask {
            friend class CLoggerBase;
        private:
            CTask( CLoggerBase &logger, bool init_succeeded = false ) :
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

            CLoggerBase& _logger;
            TLevels _out_levels;
            std::vector<SLogEntry> _log_entries;
            bool _succeeded;
        };

    public:
        friend class CTask;

        using TTasks = std::stack<CTask *>;
        using TThreads = std::map<std::thread::id, TTasks>;

        CLoggerBase() = default;
        CLoggerBase( const CLoggerBase & ) = delete;

    protected:
        const TLevels& GetLevels() const { return _out_levels; }

        const TThreads& GetThreadTasks() const { return _threads; }

        CTask AddTask( bool init_succeeded = false );
        CTask AddTask( TLevels levels, bool init_succeeded = false );

        CLoggerBase& InitLevel( std::size_t level, bool to_output );
        CLoggerBase& OnLevel( std::size_t level )  { return InitLevel(level, true); }
        CLoggerBase& OffLevel( std::size_t level ) { return InitLevel(level, false); }
        CLoggerBase& SetLevels( TLevels levels )   { _out_levels = levels; return *this; }
        bool ToBeAdded( std::size_t level ) const;

        bool ForceAddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now() );

        bool AddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now() );

    private:
        TLevels _out_levels;
        TThreads _threads;

        void RemoveTask();
    };

    template<typename TLogData>
    class CLoggerBaseThrSafety<true, TLogData>{
    protected:
        bool OutStringsThrSafe( std::size_t level, std::chrono::system_clock::time_point time, TLogData &&data );
        virtual bool OutStrings( std::size_t level, std::chrono::system_clock::time_point time, TLogData &&data ) = 0;

    private:
        std::mutex _out_mutex;
    };

    template<typename TLogData>
    class CLoggerBaseThrSafety<false, TLogData>{
    protected:
        bool OutStringsThrSafe( std::size_t level, std::chrono::system_clock::time_point time, TLogData &&data );
        virtual bool OutStrings( std::size_t level, std::chrono::system_clock::time_point time, TLogData &&data ) = 0;
    };

    template<typename TLogData>
    bool CLoggerBaseThrSafety<true, TLogData>::OutStringsThrSafe( std::size_t level, std::chrono::system_clock::time_point time, TLogData &&data ){
        std::lock_guard<decltype(_out_mutex)> lock_guard( _out_mutex );
        return OutStrings( level, time, std::move( data ));
    }

    template<typename TLogData>
    bool CLoggerBaseThrSafety<false, TLogData>::OutStringsThrSafe( std::size_t level, std::chrono::system_clock::time_point time, TLogData &&data ){
        return OutStrings( level, time, std::move( data ));
    }

    template<bool ThrSafe, typename TLogData>
    typename CLoggerBase<ThrSafe, TLogData>::CTask& CLoggerBase<ThrSafe, TLogData>::CTask::InitLevel( std::size_t level, bool to_output ) {
        if( to_output ) _out_levels.emplace( level );
        else            _out_levels.erase( level );
        return *this;
    }

    template<bool ThrSafe, typename TLogData>
    typename CLoggerBase<ThrSafe, TLogData>::CTask& CLoggerBase<ThrSafe, TLogData>::CTask::AddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time ) {
        _log_entries.emplace_back( level, std::forward<TLogData>(data), time );
        return *this;
    }

    template<bool ThrSafe, typename TLogData>
    CLoggerBase<ThrSafe, TLogData>::CTask::~CTask() {
        for( auto &entry : _log_entries ) {
            if( !_succeeded || _out_levels.count( entry._level ))
                _logger.ForceAddToLog( entry._level, std::move( entry._data ), entry._time );
        }

        _log_entries.clear();
        _logger.RemoveTask();
    }

    template<bool ThrSafe, typename TLogData>
    typename CLoggerBase<ThrSafe, TLogData>::CTask CLoggerBase<ThrSafe, TLogData>::AddTask( bool init_succeeded ) {
        CTask task( *this, init_succeeded );
        _threads[std::this_thread::get_id()].push(&task);
        return task;
    }

    template<bool ThrSafe, typename TLogData>
    typename CLoggerBase<ThrSafe, TLogData>::CTask CLoggerBase<ThrSafe, TLogData>::AddTask(TLevels levels, bool init_succeeded ) {
        auto task = AddTask( init_succeeded );
        task.SetLevels( std::forward<TLevels>(levels) );
        return task;
    }

    template<bool ThrSafe, typename TLogData>
    void CLoggerBase<ThrSafe, TLogData>::RemoveTask() {
        assert(!_threads[std::this_thread::get_id()].empty());
        _threads[std::this_thread::get_id()].pop();
    }

    template<bool ThrSafe, typename TLogData>
    CLoggerBase<ThrSafe, TLogData>& CLoggerBase<ThrSafe, TLogData>::InitLevel( std::size_t level, bool to_output ) {
        if( to_output ) _out_levels.emplace( level );
        else            _out_levels.erase( level );
        return *this;
    }

    template<bool ThrSafe, typename TLogData>
    bool CLoggerBase<ThrSafe, TLogData>::ToBeAdded( std::size_t level ) const {
        const auto thread = _threads.find(std::this_thread::get_id());
        if( thread != _threads.cend() && !thread->second.empty() )
            return true;
        else if ( _out_levels.count(level) )
            return true;
        else
            return false;
    }

    template<bool ThrSafe, typename TLogData>
    bool CLoggerBase<ThrSafe, TLogData>::AddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time ) {
        auto thread = _threads.find(std::this_thread::get_id());
        auto &tasks = thread->second;
        if( thread != _threads.end() && !tasks.empty() ) {
            auto &top = tasks.top();
            assert( top );
            top->AddToLog( level, std::forward<TLogData>(data), time );
            return true;
        } else if( ToBeAdded(level) )
            return CLoggerBaseThrSafety<ThrSafe,TLogData>::OutStringsThrSafe( level, time, std::forward<TLogData>(data) );
        else
            return false;
    }

    template<bool ThrSafe, typename TLogData>
    bool CLoggerBase<ThrSafe, TLogData>::ForceAddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time ) {
        return CLoggerBaseThrSafety<ThrSafe,TLogData>::OutStringsThrSafe( level, time, std::forward<TLogData>(data) );
    }

} // namespace Logger

#endif  // _AVN_LOGGER_BASE_H_
