//
// Created by Oleksandr Nemchenko on 11/5/19.
//

#ifndef _AVN_LOGGER_LOGGER_GROUP_TASK_H
#define _AVN_LOGGER_LOGGER_GROUP_TASK_H

#include <tuple>

namespace Logger {

    template< typename... _TTaskPtr >
    class CLoggerGroupTask {
    public:
        using _TArrayPtr = std::tuple< _TTaskPtr... >;

        CLoggerGroupTask( const _TArrayPtr &) = delete;
        CLoggerGroupTask( _TArrayPtr &&task ) : _task( std::move( task )) {std::apply( []( auto&&... task ){ ( assert( task ), ... ); }, _task ); }

        ~CLoggerGroupTask() { std::apply( []( auto&&... task ){ ( delete task, ... ); }, _task ); }

        template< size_t num > auto &       Task()        { return std::get<num>( _task ); }
        template< size_t num > const auto & Task() const  { return std::get<num>( _task ); }

        CLoggerGroupTask& SetTaskResult( bool success );
        CLoggerGroupTask& SetSuccess();
        CLoggerGroupTask& SetFail();

        CLoggerGroupTask& InitLevel( std::size_t level, bool to_output );
        CLoggerGroupTask& SetLevels( TLevels levels );
        CLoggerGroupTask& OnLevel( std::size_t level );
        CLoggerGroupTask& OffLevel( std::size_t level );

    private:
        _TArrayPtr _task;

    };  // class CLoggerGroup

    template< typename... _TTaskPtr >
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::SetTaskResult( bool success ) {
        std::apply( [success]( auto&... task ) { ( task->SetTaskResult( success ), ... ); }, _task );
        return *this;
    }

    template< typename... _TTaskPtr >
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::SetSuccess() {
        std::apply( []( auto&... task ) { ( task->SetSuccess(), ... ); }, _task );
        return *this;
    }

    template< typename... _TTaskPtr >
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::SetFail() {
        std::apply( []( auto&... task ) { ( task->SetFail(), ... ); }, _task );
        return *this;
    }

    template< typename... _TTaskPtr >
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::InitLevel( std::size_t level, bool to_output ) {
            std::apply( [level,to_output]( auto&... task ) { ( task->InitLevel( level, to_output ), ... ); }, _task );
            return *this;
    }

    template< typename... _TTaskPtr >
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::SetLevels( TLevels levels ) {
            std::apply( [&levels]( auto&... task ) { ( task->SetLevels( levels ), ... ); }, _task );
            return *this;
    }

    template< typename... _TTaskPtr >
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::OnLevel( std::size_t level ) {
            std::apply( [level]( auto&... task ) { ( task->OnLevel( level ), ... ); }, _task );
            return *this;
    }

    template< typename... _TTaskPtr >
    CLoggerGroupTask<_TTaskPtr...>& CLoggerGroupTask<_TTaskPtr...>::OffLevel( std::size_t level ) {
            std::apply( [level]( auto&... task ) { ( task->OffLevel( level ), ... ); }, _task );
            return *this;
    }

}   // namespace Logger

#endif // _AVN_LOGGER_LOGGER_GROUP_TASK_H
