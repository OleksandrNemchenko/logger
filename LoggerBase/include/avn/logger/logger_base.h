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

#include <avn/logger/data_types.h>
#include <avn/logger/base_thr_safety.h>
#include <avn/logger/task.h>

namespace Logger {

    template<bool ThrSafe, typename TLogData>
    class CLoggerBase : public CLoggerBaseThrSafety<ThrSafe, TLogData>, public CLoggerTaskInterface<TLogData>{
    public:

        using TTaskInterface = CLoggerTaskInterface<TLogData>;
        using TTasks = std::stack<CTask<TLogData> *>;
        using TThreads = std::map<std::thread::id, TTasks>;

        CLoggerBase() = default;
        CLoggerBase( const CLoggerBase & ) = delete;

        const TLevels& GetLevels() const override { return _out_levels; }

        const TThreads& GetThreadTasks() const { return _threads; }

        CTask<TLogData> AddTask( bool init_succeeded );
        CTask<TLogData> AddTask()                   { return AddTask( false ); }
        CTask<TLogData> AddTask( TLevels levels, bool init_succeeded = false );

        void InitLevel( std::size_t level, bool to_output );
        void OnLevel( std::size_t level )  { InitLevel(level, true); }
        void OffLevel( std::size_t level ) { InitLevel(level, false); }
        void SetLevels( TLevels levels )   { _out_levels = levels; }
        bool ToBeAdded( std::size_t level ) const;

        bool ForceAddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now() ) override;

        bool AddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now() );

    private:
        TLevels _out_levels;
        TThreads _threads;

        void RemoveTask() override;
    };

    template<bool ThrSafe, typename TLogData>
    CTask<TLogData> CLoggerBase<ThrSafe, TLogData>::AddTask( bool init_succeeded ) {
        auto task = TTaskInterface::CreateTask( init_succeeded );
        _threads[std::this_thread::get_id()].push(&task);
        return task;
    }

    template<bool ThrSafe, typename TLogData>
    CTask<TLogData> CLoggerBase<ThrSafe, TLogData>::AddTask(TLevels levels, bool init_succeeded ) {
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
    void CLoggerBase<ThrSafe, TLogData>::InitLevel( std::size_t level, bool to_output ) {
        if( to_output ) _out_levels.emplace( level );
        else            _out_levels.erase( level );
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
