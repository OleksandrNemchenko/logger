// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*! \file logger_txt_base.h
 * \brief CLoggerTxtBase class implements base text messages logging functionality.
 *
 * #Logger::CLoggerTxtBase class is the #Logger::CLoggerBase child that implements text messages logging. It introduces logger level
 * description map that will be used for logger message prefix. You have to specify multithreading security mode (see
 * #Logger::CLoggerBase class description) and character type. It could be char, wchar_t etc.
 *
 * You can tune output format by specifying logger message prefixes. Output string is composed by prefixes :
 *
 * `str = date (_output_format) + _space + _level_prefix + level_text + _level_postfix + _space + log_message;`
 *
 * Here _output_format is used for timestamp to string conversion as std::put_time second argument. _level_prefix and
 * _level_postfix are level name surrounds. _space is the space between different elements. You can change those texts
 * by calling #Logger::CLoggerTxtBase::SetDateOutputFormat, #Logger::CLoggerTxtBase::SetLevelPrefix, #Logger::CLoggerTxtBase::SetLevelPostfix and
 * #Logger::CLoggerTxtBase::SetSpace calls. Also you can change default values that are used for new logger instance by changing
 * #Logger::CLoggerTxtBaseDefaults members. This class has default instantion for char and wchar_t character symbol types.
 */

#ifndef _AVN_LOGGER_TXT_BASE_H_
#define _AVN_LOGGER_TXT_BASE_H_

#include <iomanip>
#include <sstream>

#include <avn/logger/logger_base.h>

namespace Logger {

/** Default values for logger msssage decorating
 *
 * \tparam _TChar Logger message symbol type
 */
template<typename _TChar>
class CLoggerTxtBaseDefaults {
public:
    using TString = std::basic_string<_TChar>;

    /** Default timestamp to string format */
    static TString _default_output_format;

    /** Default text before level descriptor */
    static TString _default_level_prefix;

    /** Default text after level descriptor */
    static TString _default_level_postfix;

    /** Default space text */
    static TString _default_space;
};

/** Base class for text loggers
 * 
 * \tparam _ThrSafe Thread security mode. If true, multithread mode will be used. Otherwise single ther will be activated.
 * \tparam _TChar Character data type. Can be char, wchar_t etc.
 */
template<bool _ThrSafe, typename _TChar>
class CLoggerTxtBase : public CLoggerBase<_ThrSafe, std::basic_string<_TChar>>, public CLoggerTxtBaseDefaults<_TChar> {
public :
    /** Current thread security mode. If true, multithread mode will be used. Otherwise single ther will be activated */
    constexpr static bool ThrSafe{ _ThrSafe };

    /** Character type for text logger messages */
    using TChar = _TChar;

    /** String type for text logger messages */
    using TString = std::basic_string<TChar>;

    /** Levels mapping type */
    using TLevelsMap = std::map<size_t, TString>;

private :
    using TBase = CLoggerBase<_ThrSafe, TString>;
    using TDef = CLoggerTxtBaseDefaults<_TChar>;

public :
    /** Default constructor
     *
     * \param local_time Local time will be used for timestamp. By default it is true
     */
    CLoggerTxtBase( bool local_time = true );

    /** Add level descriptor
     *
     * \param level Level identifier
     * \param name Level descriptor
     *
     * \return Current instance reference
     */
    CLoggerTxtBase& AddLevelDescr( size_t level, TString name )     { _levels_map[level] = std::forward<TString>( name ); return *this; }

    /** Return levels map */
    const TLevelsMap& LevelsMap() const                             { return _levels_map; }

    /** Output the text message arguments
    *
    * If a task is active, message will be logged. If no task is active, message will be output
    * only if logger level is enabled.
    *
    * Message will be output with the current timestamp.
    *
    * \tparam T Message elements types.
    * \warning Each type must be able to to be used as argument for
    * std::basic_stringstream<TChar>::operator<<( std::forward<T>(args) ) call
    *
    * \param level Level identifier
    * \param args Arguments
    *
    * \return Current instance reference
    */
    template<typename... T>
    CLoggerTxtBase& AddString( std::size_t level, T&&... args );

    /** Output the text message arguments
    *
    * If a task is active, message will be logged. If no task is active, message will be output
    * only if logger level is enabled.
    *
    * \tparam T Message elements types.
    * \warning Each type must be able to to be used as argument for
    * std::basic_stringstream<TChar>::operator<<( std::forward<T>(args) ) call
    *
    * \param time
    * \param level Level identifier
    * \param args Arguments
    *
    * \return Current instance reference
    */
    template<typename... T>
    CLoggerTxtBase& AddString( std::chrono::system_clock::time_point time, std::size_t level, T&&... args );

    /** Set timestamp to string format
     *
     * \param output_format std::put_time timestamp format
     *
     * \return Current instance reference
     */
    CLoggerTxtBase& SetDateOutputFormat( TString &&output_format )  { _output_format = std::forward<TString>( output_format ); return *this; }

    /** Default text before level descriptor
     *
     * \param level_prefix Level prefix
     *
     * \return Current instance reference
     */
    CLoggerTxtBase& SetLevelPrefix( TString &&level_prefix )        { _level_prefix  = std::forward<TString>( level_prefix );  return *this; }

    /** Default text after level descriptor
     *
     * \param level_postfix Level postfix
     *
     * \return Current instance reference
     */
    CLoggerTxtBase& SetLevelPostfix( TString &&level_postfix )      { _level_postfix = std::forward<TString>( level_postfix ); return *this; }

    /** Default space text
     *
     * \param space Space between different elements
     *
     * \return Current instance reference
     */
    CLoggerTxtBase& SetSpace( TString &&space )                     { _space         = std::forward<TString>( space );         return *this; }

    /** Output the text message arguments
    *
    * If a task is active, message will be logged. If no task is active, message will be output
    * only if logger level is enabled.
    *
    * Message will be output with the current timestamp.
    *
    * \tparam T Message elements types.
    * \warning Each type must be able to to be used as argument for
    * std::basic_stringstream<TChar>::operator<<( std::forward<T>(args) ) call
    *
    * \param level Level identifier
    * \param args Arguments
    *
    * \return Current instance reference
    */
    template<typename... T>
    CLoggerTxtBase& operator() ( std::size_t level, T&&... args )    { return AddString( level, args... ); }

    /** Output the text message arguments
    *
    * If a task is active, message will be logged. If no task is active, message will be output
    * only if logger level is enabled.
    *
    * \tparam T Message elements types.
    * \warning Each type must be able to to be used as argument for
    * std::basic_stringstream<TChar>::operator<<( std::forward<T>(args) ) call
    *
    * \param time
    * \param level Level identifier
    * \param args Arguments
    *
    * \return Current instance reference
    */
    template<typename... T>
    CLoggerTxtBase& operator() ( std::chrono::system_clock::time_point time, std::size_t level, T&&... args )    { return AddString( time, level, args... ); }

protected:
    /** Decorate string
     *
     * This function decorates string by using prefixe, postfix, timestamp format and space string. It is intented
     * to be called by children classes.
     *
     * \param level Level identifier
     * \param time Message timestamp
     * \param data Message string
     *
     * \return
     */
    TString PrepareString( std::size_t level, std::chrono::system_clock::time_point time, const TString &data ) const;

private:
    TLevelsMap _levels_map;
    std::function<std::tm* ( const std::time_t* )> _time_converter;
    TString _output_format;
    TString _level_prefix;
    TString _level_postfix;
    TString _space;

};

template<bool _ThrSafe, typename _TChar>
CLoggerTxtBase<_ThrSafe, _TChar>::CLoggerTxtBase( bool local_time ):
    _time_converter( local_time ? std::localtime : std::gmtime ),
    _output_format{ TDef::_default_output_format },
    _level_prefix{  TDef::_default_level_prefix  },
    _level_postfix{ TDef::_default_level_postfix },
    _space{         TDef::_default_space         }
    { }

template<bool _ThrSafe, typename _TChar>
template<typename... T>
CLoggerTxtBase<_ThrSafe, _TChar>& CLoggerTxtBase<_ThrSafe, _TChar>::AddString( std::chrono::system_clock::time_point time, std::size_t level, T&&... args ) {
    if( !TBase::TaskOrToBeAdded(level) )
        return *this;
    std::basic_stringstream<_TChar> stream;
    ( stream << ... << std::forward<T>(args) );
    TBase::AddToLog( level, stream.str(), time );
    return *this;
}

template<bool _ThrSafe, typename _TChar>
template<typename... T>
CLoggerTxtBase<_ThrSafe, _TChar>& CLoggerTxtBase<_ThrSafe, _TChar>::AddString( std::size_t level, T&&... args ) {
    return AddString( std::chrono::system_clock::now(), level, args... );
}

template<bool _ThrSafe, typename _TChar>
typename CLoggerTxtBase<_ThrSafe, _TChar>::TString CLoggerTxtBase<_ThrSafe, _TChar>::PrepareString( std::size_t level, std::chrono::system_clock::time_point time, const TString &data ) const {
    const auto level_it = _levels_map.find(level);
    TString str;

    assert( level_it != _levels_map.cend() );

    std::time_t time_moment = std::chrono::system_clock::to_time_t( time );
    std::basic_stringstream<_TChar> sstr;
    sstr << std::put_time(_time_converter( &time_moment ), _output_format.c_str() );
    str = sstr.str() + _space + _level_prefix + level_it->second + _level_postfix + _space + data;

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
