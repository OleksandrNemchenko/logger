// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef _AVN_LOGGER_TXT_BASE_H_
#define _AVN_LOGGER_TXT_BASE_H_

#include <iomanip>
#include <sstream>

#include <avn/logger/logger_base.h>

namespace Logger {

template<typename TChar>
class CLoggerTxtBaseDefaults {
public:
    using TString = std::basic_string<TChar>;

    static TString _default_output_format;
    static TString _default_level_prefix;
    static TString _default_level_postfix;
    static TString _default_space;
};

template<bool LockThr, typename TChar>
class CLoggerTxtBase : public CLoggerBase<LockThr, std::basic_string<TChar>>, public CLoggerTxtBaseDefaults<TChar> {
public:
    using TString = std::basic_string<TChar>;
    using TBase = CLoggerBase<LockThr, TString>;
    using TDef = CLoggerTxtBaseDefaults<TChar>;
    using TLevelsMap = std::map<size_t, TString>;

    CLoggerTxtBase( bool local_time = true );

    CLoggerTxtBase& AddLevelDescr( size_t level, TString name )     { _levels[level] = std::forward<TString>( name ); return *this; }
    CLoggerTxtBase& InitLevel( std::size_t level, bool to_output )  { TBase::InitLevel( level, to_output ); return *this; }
    CLoggerTxtBase& OnLevel( std::size_t level )                    { TBase::OnLevel( level );              return *this; }
    CLoggerTxtBase& SetLevels( TLevels levels )                     { TBase::SetLevels( levels );           return *this; }
    CLoggerTxtBase& OffLevel( std::size_t level )                   { TBase::OffLevel( level );             return *this; }
    const TLevelsMap& Levels() const                                { return _levels; }
    CTask<TString> AddTask( bool init_succeeded = false )                   { return TBase::AddTask( init_succeeded ); }
    CTask<TString> AddTask( TLevels levels, bool init_succeeded = false )   { return TBase::AddTask( levels, init_succeeded ); }
    CLoggerTxtBase& FinishTask( bool success )                      { TBase::FinishTask( success ); return *this; }

    template<typename... T>
    CLoggerTxtBase& AddString( std::size_t level, T&&... args );

    template<typename... T>
    CLoggerTxtBase& AddString( std::chrono::system_clock::time_point time, std::size_t level, T&&... args );

    CLoggerTxtBase& SetDateOutputFormat( TString &&output_format )  { _output_format = std::forward<TString>( output_format ); return *this; }
    CLoggerTxtBase& SetLevelPrefix( TString &&level_prefix )        { _level_prefix  = std::forward<TString>( level_prefix );  return *this; }
    CLoggerTxtBase& SetLevelPostfix( TString &&level_postfix )      { _level_postfix = std::forward<TString>( level_postfix ); return *this; }
    CLoggerTxtBase& SetSpace( TString &&space )                     { _space         = std::forward<TString>( space );         return *this; }

    template<typename... T>
    CLoggerTxtBase& operator() ( std::size_t level, T&&... args )    { return AddString( level, args... ); }

    template<typename... T>
    CLoggerTxtBase& operator() ( std::chrono::system_clock::time_point time, std::size_t level, T&&... args )    { return AddString( time, level, args... ); }

protected:
    TString PrepareString( std::size_t level, std::chrono::system_clock::time_point time, TString &&data ) const;

private:
    TLevelsMap _levels;
    std::function<std::tm* ( const std::time_t* )> _time_converter;
    TString _output_format;
    TString _level_prefix;
    TString _level_postfix;
    TString _space;

};

template<bool LockThr, typename TChar>
CLoggerTxtBase<LockThr, TChar>::CLoggerTxtBase( bool local_time ):
    _time_converter( local_time ? std::localtime : std::gmtime ),
    _output_format{ TDef::_default_output_format },
    _level_prefix{  TDef::_default_level_prefix  },
    _level_postfix{ TDef::_default_level_postfix },
    _space{         TDef::_default_space         }
    { }

template<bool LockThr, typename TChar>
template<typename... T>
CLoggerTxtBase<LockThr, TChar>& CLoggerTxtBase<LockThr, TChar>::AddString( std::chrono::system_clock::time_point time, std::size_t level, T&&... args ) {
    if( !TBase::ToBeAdded(level) )
        return *this;
    std::basic_stringstream<TChar> stream;
    ( stream << ... << std::forward<T>(args) );
    TBase::AddToLog( level, stream.str(), time );
    return *this;
}

template<bool LockThr, typename TChar>
template<typename... T>
CLoggerTxtBase<LockThr, TChar>& CLoggerTxtBase<LockThr, TChar>::AddString( std::size_t level, T&&... args ) {
    return AddString( std::chrono::system_clock::now(), level, args... );
}

template<bool LockThr, typename TChar>
typename CLoggerTxtBase<LockThr, TChar>::TString CLoggerTxtBase<LockThr, TChar>::PrepareString( std::size_t level, std::chrono::system_clock::time_point time, TString &&data ) const {
    const auto level_it = _levels.find(level);
    TString str;

    assert( level_it != _levels.cend() );

    std::time_t time_moment = std::chrono::system_clock::to_time_t( time );
    std::basic_stringstream<TChar> sstr;
    sstr << std::put_time(_time_converter( &time_moment ), _output_format.c_str() );
    str = sstr.str() + _space + _level_prefix + level_it->second + _level_postfix + _space + std::forward<TString>( data );

    return str;
}

template<> inline std::string CLoggerTxtBaseDefaults<char>::_default_output_format{ "%F %T" };
template<> inline std::string CLoggerTxtBaseDefaults<char>::_default_level_prefix { "[" };
template<> inline std::string CLoggerTxtBaseDefaults<char>::_default_level_postfix{ "]" };
template<> inline std::string CLoggerTxtBaseDefaults<char>::_default_space        { " " };

template<> inline std::wstring CLoggerTxtBaseDefaults<wchar_t>::_default_output_format{ L"%F %T" };
template<> inline std::wstring CLoggerTxtBaseDefaults<wchar_t>::_default_level_prefix { L"[" };
template<> inline std::wstring CLoggerTxtBaseDefaults<wchar_t>::_default_level_postfix{ L"]" };
template<> inline std::wstring CLoggerTxtBaseDefaults<wchar_t>::_default_space        { L" " };

} // namespace Logger

#endif  // _AVN_LOGGER_TXT_BASE_H_
