
#pragma once

#include <ctime>
#include <functional>
#include <fstream>
#include <iomanip>
#include <string>

#include <avn/logger/logger_base.h>

namespace Logger {

template<typename TChar>
class CLoggerTxtBase : public CLoggerBase<std::basic_string<TChar>> {
public:
    using TBase = CLoggerBase<std::basic_string<TChar>>;
    using TString = std::basic_string<TChar>;
    using TLogData = typename TBase::TLogData;
    using TLevels = typename TBase::TLevels;

    CLoggerTxtBase( bool local_time = true, TString &&output_format = TString{"%F %T"}, TString &&level_prefix = TString{"["}, TString &&level_postfix = TString{"]"} );
    CLoggerTxtBase( const std::filesystem::path &filename, bool local_time = true, std::ios_base::openmode mode = std::ios_base::out, TString &&output_format = TString{"%F %T"}, TString &&level_prefix = TString{"["}, TString &&level_postfix = TString{"]"} ) :
        CLoggerTxtBase( local_time, std::forward<TString>(output_format), std::forward<TString>(level_prefix), std::forward<TString>(level_postfix) )
    { OpenFile( filename, mode ); }

    CLoggerTxtBase& OpenFile( const std::filesystem::path &filename, std::ios_base::openmode mode = std::ios_base::out )    { _fstream.open( filename, mode ); return *this; }
    bool IsOpenedFile( void ) const     { return _fstream.is_open(); }
    CLoggerTxtBase& CloseFile( const std::filesystem::path &filename, std::ios_base::openmode mode = std::ios_base::out )    { _fstream.close(); return *this; }
    CLoggerTxtBase& FlushFile( void )   { _fstream.flush(); return *this; }

    CLoggerTxtBase& AddLevelDescr( size_t level, TString &&name )   { _levels[level] = std::forward<TString>( name ); return *this; }
    CLoggerTxtBase& InitLevel( std::size_t level, bool to_output )  { TBase::InitLevel( level, to_output ); return *this; }
    CLoggerTxtBase& OnLevel( std::size_t level )                    { TBase::OnLevel( level );              return *this; }
    CLoggerTxtBase& SetLevels( TLevels levels )                     { TBase::SetLevels( levels );           return *this; }
    CLoggerTxtBase& OffLevel( std::size_t level )                   { TBase::OffLevel( level );             return *this; }

    CLoggerTxtBase& AddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now() );

private:

    bool OutStrings( std::size_t level, std::chrono::system_clock::time_point time, TLogData &&data ) override final;

    std::basic_ofstream<TChar> _fstream;
    std::unordered_map<size_t, TString> _levels;
    std::function<std::tm* ( const std::time_t* )> _time_converter;
    TString _output_format;
    TString _level_prefix;
    TString _level_postfix;

};

template<typename TChar>
CLoggerTxtBase<TChar>::CLoggerTxtBase( bool local_time, TString &&output_format, TString &&level_prefix, TString &&level_postfix ):
    _time_converter( local_time ? std::localtime : std::gmtime ),
    _output_format( std::forward<TString>( output_format )),
    _level_prefix( std::forward<TString>( level_prefix )),
    _level_postfix( std::forward<TString>( level_postfix ))
    { }

template<typename TChar>
bool CLoggerTxtBase<TChar>::OutStrings( std::size_t level, std::chrono::system_clock::time_point time, TLogData &&data ){
    const auto level_it = _levels.find(level);
    static const auto space = TString( " " );

    assert( _fstream.is_open() && level_it != _levels.cend() );

    if( _fstream.is_open() ){
        std::time_t time_moment = std::chrono::system_clock::to_time_t( time );
        _fstream << std::put_time(_time_converter( &time_moment ), _output_format.c_str() ) <<
                 space << _level_prefix << level_it->second << _level_postfix;

        for( const auto &item : std::forward<TLogData>( data ))
            _fstream << space << item;

        _fstream << std::endl;

        return true;
    }

    return false;
}

template<typename TChar>
CLoggerTxtBase<TChar>& CLoggerTxtBase<TChar>::AddToLog( std::size_t level, TLogData &&data, std::chrono::system_clock::time_point time ) {
    TBase::AddToLog( level, std::forward<TLogData>( data ), time );
    return *this;
}

using CLoggerTxt = CLoggerTxtBase<char>;
using CLoggerWTxt = CLoggerTxtBase<wchar_t>;

} // namespace Logger