
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
    using TLevels = typename TBase::TLevels;

    CLoggerTxtBase( bool local_time = true );
    CLoggerTxtBase( const std::filesystem::path &filename, bool local_time = true, std::ios_base::openmode mode = std::ios_base::out ) :
        CLoggerTxtBase( local_time )
    { OpenFile( filename, mode ); }

    CLoggerTxtBase& OpenFile( const std::filesystem::path &filename, std::ios_base::openmode mode = std::ios_base::out )    { _fstream.open( filename, mode ); return *this; }
    bool IsOpenedFile( void ) const                                 { return _fstream.is_open(); }
    CLoggerTxtBase& CloseFile( const std::filesystem::path &filename, std::ios_base::openmode mode = std::ios_base::out )    { _fstream.close(); return *this; }
    CLoggerTxtBase& FlushFile( void )                               { _fstream.flush(); return *this; }
    CLoggerTxtBase& Imbue( const std::locale& loc )                 { _fstream.imbue( loc ); return *this; }

    CLoggerTxtBase& AddLevelDescr( size_t level, TString &&name )   { _levels[level] = std::forward<TString>( name ); return *this; }
    CLoggerTxtBase& InitLevel( std::size_t level, bool to_output )  { TBase::InitLevel( level, to_output ); return *this; }
    CLoggerTxtBase& OnLevel( std::size_t level )                    { TBase::OnLevel( level );              return *this; }
    CLoggerTxtBase& SetLevels( TLevels levels )                     { TBase::SetLevels( levels );           return *this; }
    CLoggerTxtBase& OffLevel( std::size_t level )                   { TBase::OffLevel( level );             return *this; }

    CLoggerTxtBase& AddToLog( std::size_t level, TString &&data, std::chrono::system_clock::time_point time = std::chrono::system_clock::now() );

    CLoggerTxtBase& SetDateOutputFormat( TString &&output_format )  { _output_format = std::forward<TString>( output_format ); return *this; }
    CLoggerTxtBase& SetLevelPrefix( TString &&level_prefix )        { _level_prefix  = std::forward<TString>( level_prefix );  return *this; }
    CLoggerTxtBase& SetLevelPostfix( TString &&level_postfix )      { _level_postfix = std::forward<TString>( level_postfix ); return *this; }
    CLoggerTxtBase& SetSpace( TString &&space )                     { _space         = std::forward<TString>( space );         return *this; }

private:

    bool OutStrings( std::size_t level, std::chrono::system_clock::time_point time, TString &&data ) override final;

    std::basic_ofstream<TChar> _fstream;
    std::unordered_map<size_t, TString> _levels;
    std::function<std::tm* ( const std::time_t* )> _time_converter;
    TString _output_format;
    TString _level_prefix;
    TString _level_postfix;
    TString _space;

public:
    static TString _default_output_format;
    static TString _default_level_prefix;
    static TString _default_level_postfix;
    static TString _default_space;

};

template<typename TChar>
CLoggerTxtBase<TChar>::CLoggerTxtBase( bool local_time ):
    _time_converter( local_time ? std::localtime : std::gmtime ),
    _output_format( _default_output_format ),
    _level_prefix(  _default_level_prefix  ),
    _level_postfix( _default_level_postfix ),
    _space(         _default_space )
    { }

template<typename TChar>
bool CLoggerTxtBase<TChar>::OutStrings( std::size_t level, std::chrono::system_clock::time_point time, TString &&data ){
    const auto level_it = _levels.find(level);

    assert( _fstream.is_open() && level_it != _levels.cend() );

    if( _fstream.is_open() ){
        std::time_t time_moment = std::chrono::system_clock::to_time_t( time );
        _fstream << std::put_time(_time_converter( &time_moment ), _output_format.c_str() );
        _fstream << _space << _level_prefix << level_it->second << _level_postfix;

        for( const auto &item : std::forward<TString>( data ))
            _fstream << _space << item;

        _fstream << std::endl;

        return true;
    }

    return false;
}

template<typename TChar>
CLoggerTxtBase<TChar>& CLoggerTxtBase<TChar>::AddToLog( std::size_t level, TString &&data, std::chrono::system_clock::time_point time ) {
    TBase::AddToLog( level, std::forward<TString>( data ), time );
    return *this;
}

using CLoggerTxt = CLoggerTxtBase<char>;
template<> std::basic_string<char> CLoggerTxt::_default_output_format{ "%F %T" };
template<> std::basic_string<char> CLoggerTxt::_default_level_prefix { "[" };
template<> std::basic_string<char> CLoggerTxt::_default_level_postfix{ "]" };
template<> std::basic_string<char> CLoggerTxt::_default_space        { " " };

using CLoggerWTxt = CLoggerTxtBase<wchar_t>;
template<> std::basic_string<wchar_t> CLoggerWTxt::_default_output_format{ L"%F %T" };
template<> std::basic_string<wchar_t> CLoggerWTxt::_default_level_prefix { L"[" };
template<> std::basic_string<wchar_t> CLoggerWTxt::_default_level_postfix{ L"]" };
template<> std::basic_string<wchar_t> CLoggerWTxt::_default_space        { L" " };

} // namespace Logger