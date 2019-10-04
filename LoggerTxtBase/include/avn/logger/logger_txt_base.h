
#ifndef _AVN_LOGGER_TXT_BASE_H_
#define _AVN_LOGGER_TXT_BASE_H_

#include <sstream>

#include <avn/logger/logger_base.h>

namespace Logger {

template<typename TChar>
class CLoggerTxtBase : public CLoggerBase<std::basic_string<TChar>> {
public:
    using TBase = CLoggerBase<std::basic_string<TChar>>;
    using TString = std::basic_string<TChar>;
    using TLevels = typename TBase::TLevels;
    using TLevelsMap = std::unordered_map<size_t, TString>;

    CLoggerTxtBase( bool local_time = true );

    CLoggerTxtBase& AddLevelDescr( size_t level, TString &&name )   { _levels[level] = std::forward<TString>( name ); return *this; }
    CLoggerTxtBase& InitLevel( std::size_t level, bool to_output )  { TBase::InitLevel( level, to_output ); return *this; }
    CLoggerTxtBase& OnLevel( std::size_t level )                    { TBase::OnLevel( level );              return *this; }
    CLoggerTxtBase& SetLevels( TLevels levels )                     { TBase::SetLevels( levels );           return *this; }
    CLoggerTxtBase& OffLevel( std::size_t level )                   { TBase::OffLevel( level );             return *this; }
    const TLevelsMap& Levels() const                                { return _levels; }

    template<typename... T>
    CLoggerTxtBase& AddString( std::size_t level, T&&... args);

    template<typename... T>
    CLoggerTxtBase& AddString( std::chrono::system_clock::time_point time, std::size_t level, T&&... args );

    CLoggerTxtBase& SetDateOutputFormat( TString &&output_format )  { _output_format = std::forward<TString>( output_format ); return *this; }
    CLoggerTxtBase& SetLevelPrefix( TString &&level_prefix )        { _level_prefix  = std::forward<TString>( level_prefix );  return *this; }
    CLoggerTxtBase& SetLevelPostfix( TString &&level_postfix )      { _level_postfix = std::forward<TString>( level_postfix ); return *this; }
    CLoggerTxtBase& SetSpace( TString &&space )                     { _space         = std::forward<TString>( space );         return *this; }

protected:
    TString PrepareString( std::size_t level, std::chrono::system_clock::time_point time, TString &&data ) const;

private:

    TLevelsMap _levels;
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
template<typename... T>
CLoggerTxtBase<TChar>& CLoggerTxtBase<TChar>::AddString( std::chrono::system_clock::time_point time, std::size_t level, T&&... args ) {
    if( !TBase::ToBeAdded(level) )
        return *this;
    std::basic_stringstream<TChar> stream;
    ( stream << ... << std::forward<T>(args) );
    TBase::AddToLog( level, stream.str(), time );
    return *this;
}

template<typename TChar>
template<typename... T>
CLoggerTxtBase<TChar>& CLoggerTxtBase<TChar>::AddString( std::size_t level, T&&... args ) {
    return AddString( std::chrono::system_clock::now(), level, args... );
}

template<typename TChar>
typename CLoggerTxtBase<TChar>::TString CLoggerTxtBase<TChar>::PrepareString( std::size_t level, std::chrono::system_clock::time_point time, TString &&data ) const {
    const auto level_it = _levels.find(level);
    TString str;

    assert( level_it != _levels.cend() );

    std::time_t time_moment = std::chrono::system_clock::to_time_t( time );
    std::basic_stringstream<TChar> sstr;
    sstr << std::put_time(_time_converter( &time_moment ), _output_format.c_str() );
    str = sstr.str() + _space + _level_prefix + level_it->second + _level_postfix + _space + std::forward<TString>( data );

    return str;
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

#endif  // _AVN_LOGGER_TXT_BASE_H_
