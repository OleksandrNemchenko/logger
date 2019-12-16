//
// Created by Oleksandr Nemchenko on 11/5/19.
//

#ifndef _AVN_LOGGER_LOGGER_TXT_GROUP_H
#define _AVN_LOGGER_LOGGER_TXT_GROUP_H

#include <avn/logger/logger_group.h>

namespace Logger {

    template< typename... _TLogger >
    class CLoggerTxtGroup : public CLoggerGroup<_TLogger...> {
    private:
        using TBase = CLoggerGroup<_TLogger...>;
        using TArray = std::tuple<_TLogger...>;
    public :
        using TString = typename std::tuple_element_t<0, TArray>::TString;
        using TLevelsMap = std::map<size_t, TString>;

        void AddLevelDescr( size_t level, TString name );

        template<typename... T>
        void AddString( std::size_t level, const T&... args );

        template<typename... T>
        void AddString( std::chrono::system_clock::time_point time, std::size_t level, const T&... args );

        void SetDateOutputFormat( const TString& output_format );
        void SetLevelPrefix( const TString& level_prefix );
        void SetLevelPostfix( const TString& level_postfix );
        void SetSpace( const TString& space );

        template<typename... T>
        void operator() ( std::size_t level, const T&... args )    { AddString( level, args... ); }

        template<typename... T>
        void operator() ( std::chrono::system_clock::time_point time, std::size_t level, const T&... args )    { AddString( time, level, args... ); }

    };  // class CLoggerTxtGroup

    template< typename... _TLogger >
    void CLoggerTxtGroup<_TLogger...>::AddLevelDescr( size_t level, TString name ) {
        std::apply( [level,name] ( auto&... logger ) { ( logger.AddLevelDescr( level, name ), ... ); }, TBase::_logger );
    }

    template< typename... _TLogger >
    template<typename... T>
    void CLoggerTxtGroup<_TLogger...>::AddString( std::size_t level, const T&... args ) {
        std::apply( [level,args...] ( auto&... logger ) { ( logger.AddString( level, args... ), ... ); }, TBase::_logger );
    }

    template< typename... _TLogger >
    template<typename... T>
    void CLoggerTxtGroup<_TLogger...>::AddString( std::chrono::system_clock::time_point time, std::size_t level, const T&... args ) {
        std::apply( [time,level,args...] ( auto&... logger ) { ( logger.AddString( time, level, args... ), ... ); }, TBase::_logger );
    }

    template< typename... _TLogger >
    void CLoggerTxtGroup<_TLogger...>::SetDateOutputFormat( const TString& output_format ) {
        std::apply( [output_format] ( auto&... logger ) { ( logger.SetDateOutputFormat( output_format ), ... ); }, TBase::_logger );
    }

    template< typename... _TLogger >
    void CLoggerTxtGroup<_TLogger...>::SetLevelPrefix( const TString& level_prefix ) {
        std::apply( [level_prefix] ( auto&... logger ) { ( logger.SetLevelPrefix( level_prefix ), ... ); }, TBase::_logger );
    }

    template< typename... _TLogger >
    void CLoggerTxtGroup<_TLogger...>::SetLevelPostfix( const TString& level_postfix ) {
        std::apply( [level_postfix] ( auto&... logger ) { ( logger.SetLevelPostfix( level_postfix ), ... ); }, TBase::_logger );
    }

    template< typename... _TLogger >
    void CLoggerTxtGroup<_TLogger...>::SetSpace( const TString& space ) {
        std::apply( [space] ( auto&... logger ) { ( logger.SetSpace( space ), ... ); }, TBase::_logger );
    }

}   // namespace Logger

#endif // _AVN_LOGGER_LOGGER_GROUP_H
