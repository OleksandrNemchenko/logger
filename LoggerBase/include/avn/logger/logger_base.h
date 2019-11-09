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

    template<bool _ThrSafe, typename _TLogData>
    class CLoggerBase : public CLoggerBaseThrSafety<_ThrSafe, _TLogData>, public CLoggerTaskInterface<_TLogData>{
    public:

        constexpr static bool ThrSafe{ _ThrSafe };
        using TLogData = _TLogData;
        using TTaskInterface = CLoggerTaskInterface<_TLogData>;
        using TTasks = std::stack<CTask<_TLogData> *>;
        using TThreads = std::map<std::thread::id, TTasks>;

        CLoggerBase() = default;
        CLoggerBase( const CLoggerBase & ) = delete;
        virtual ~CLoggerBase();

        const TLevels& GetLevels() const override { return _out_levels; }

        const TThreads& GetThreadsTasks() const { return _threads; }

        CTask<_TLogData> AddTask( bool init_succeeded );
        CTask<_TLogData> AddTask()                   { return AddTask( false ); }
        CTask<_TLogData> AddTask( TLevels levels, bool init_succeeded );
        CTask<_TLogData> AddTask( TLevels levels )   { return AddTask( levels, false ); }


        void InitLevel( std::size_t level, bool to_output );
        void OnLevel( std::size_t level )  { InitLevel(level, true); }
        void OffLevel( std::size_t level ) { InitLevel(level, false); }
        void SetLevels( TLevels levels )   { _out_levels = levels; }
        bool ToBeAdded( std::size_t level ) const;

        bool ForceAddToLog( std::size_t level, _TLogData &&data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now() ) override;

        bool AddToLog( std::size_t level, _TLogData &&data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now() );

        CTask<_TLogData> * AddTaskDynamically( bool init_succeeded ) override;

    private:
        TLevels _out_levels;
        TThreads _threads;

        void RemoveTask() override;


    };

    template<bool _ThrSafe, typename _TLogData>
    CLoggerBase<_ThrSafe, _TLogData>::~CLoggerBase(){
        for( auto &[thread_id, tasks] : _threads )
            assert( tasks.empty() );
    }

    template<bool _ThrSafe, typename _TLogData>
    CTask<_TLogData> CLoggerBase<_ThrSafe, _TLogData>::AddTask( bool init_succeeded ) {
        auto task = TTaskInterface::CreateTask( init_succeeded );
        _threads[std::this_thread::get_id()].push(&task);
        return task;
    }

    template<bool _ThrSafe, typename _TLogData>
    CTask<_TLogData> CLoggerBase<_ThrSafe, _TLogData>::AddTask(TLevels levels, bool init_succeeded ) {
        auto task = AddTask( init_succeeded );
        task.SetLevels( std::forward<TLevels>(levels) );
        return task;
    }

    template<bool _ThrSafe, typename _TLogData>
    CTask<_TLogData> * CLoggerBase<_ThrSafe, _TLogData>::AddTaskDynamically( bool init_succeeded ) {
        auto task = TTaskInterface::CreateTaskDynamically( init_succeeded );
        _threads[std::this_thread::get_id()].push(task);
        return task;
    }

    template<bool _ThrSafe, typename _TLogData>
    void CLoggerBase<_ThrSafe, _TLogData>::RemoveTask() {
        assert(!_threads[std::this_thread::get_id()].empty());
        _threads[std::this_thread::get_id()].pop();
    }

    template<bool _ThrSafe, typename _TLogData>
    void CLoggerBase<_ThrSafe, _TLogData>::InitLevel( std::size_t level, bool to_output ) {
        if( to_output ) _out_levels.emplace( level );
        else            _out_levels.erase( level );
    }

    template<bool _ThrSafe, typename _TLogData>
    bool CLoggerBase<_ThrSafe, _TLogData>::ToBeAdded( std::size_t level ) const {
        const auto thread = _threads.find(std::this_thread::get_id());
        if( thread != _threads.cend() && !thread->second.empty() )
            return true;
        else if ( _out_levels.count(level) )
            return true;
        else
            return false;
    }

    template<bool _ThrSafe, typename _TLogData>
    bool CLoggerBase<_ThrSafe, _TLogData>::AddToLog( std::size_t level, _TLogData &&data, std::chrono::system_clock::time_point time ) {
        auto thread = _threads.find(std::this_thread::get_id());
        auto &tasks = thread->second;
        if( thread != _threads.end() && !tasks.empty() ) {
            auto &top = tasks.top();
            assert( top );
            top->AddToLog( level, std::forward<_TLogData>(data), time );
            return true;
        } else if( ToBeAdded(level) )
            return CLoggerBaseThrSafety<_ThrSafe,_TLogData>::OutStringsThrSafe( level, time, std::forward<_TLogData>(data) );
        else
            return false;
    }

    template<bool _ThrSafe, typename _TLogData>
    bool CLoggerBase<_ThrSafe, _TLogData>::ForceAddToLog( std::size_t level, _TLogData &&data, std::chrono::system_clock::time_point time ) {
        return CLoggerBaseThrSafety<_ThrSafe,_TLogData>::OutStringsThrSafe( level, time, std::forward<_TLogData>(data) );
    }

} // namespace Logger

#endif  // _AVN_LOGGER_BASE_H_
