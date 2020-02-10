// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*! \file logger_txt_base.h
 * \brief ALoggerTxtBase class implements base text messages logging functionality.
 *
 * #ALogger::ALoggerTxtBase class is the #ALogger::ALoggerBase child that implements text messages logging. It introduces logger level
 * description map that will be used for logger message prefix. You have to specify multithreading security mode (see
 * #ALogger::ALoggerBase class description) and character type. It could be char, wchar_t etc.
 *
 * You can tune output format by specifying logger message prefixes. Output string is composed by prefixes :
 *
 * `str = date (_outputFormat) + _space + _levelPrefix + level_text + _levelPostfix + _space + log_message;`
 *
 * Here _outputFormat is used for timestamp to string conversion as std::put_time second argument. _levelPrefix and
 * _levelPostfix are level name surrounds. _space is the space between different elements. You can change those texts
 * by calling #ALogger::ALoggerTxtBase::setDateOutputFormat, #ALogger::ALoggerTxtBase::setLevelPrefix, #ALogger::ALoggerTxtBase::setLevelPostfix and
 * #ALogger::ALoggerTxtBase::setSpace calls. Also you can change default values that are used for new logger instance by changing
 * #ALogger::ALoggerTxtBaseDefaults members. This class has default instantion for char and wchar_t character symbol types.
 */

#ifndef _AVN_LOGGER_TXT_BASE_H_
#define _AVN_LOGGER_TXT_BASE_H_

#include <iomanip>
#include <sstream>

#include <avn/logger/logger_base.h>

namespace ALogger {

    /** Default values for logger msssage decorating
     *
     * \tparam _TChar ALogger message symbol type
     */
    template<typename _TChar>
    class ALoggerTxtBaseDefaults {
    public:
        using TString = std::basic_string<_TChar>;

        /** Default timestamp to string format */
        static TString _defaultOutputFormat;

        /** Default text before level descriptor */
        static TString _defaultLevelPrefix;

        /** Default text after level descriptor */
        static TString _defaultLevelPostfix;

        /** Default space text */
        static TString _defaultSpace;
    };

    /** Base class for text loggers
     *
     * \tparam _ThrSafe Thread security mode. If true, multithread mode will be used. Otherwise single ther will be activated.
     * \tparam _TChar Character data type. Can be char, wchar_t etc.
     */
    template<bool _ThrSafe, typename _TChar>
    class ALoggerTxtBase : public ALoggerBase<_ThrSafe, std::basic_string<_TChar>>, public ALoggerTxtBaseDefaults<_TChar> {
    public :
        /** Current thread security mode. If true, multithread mode will be used. Otherwise single ther will be activated */
        constexpr static bool ThrSafe{ _ThrSafe };

        /** Character type for text logger messages */
        using TChar = _TChar;

        /** String type for text logger messages */
        using TString = std::basic_string<TChar>;

        /** Levels mapping type */
        using TlevelsMap = std::map<size_t, TString>;

    private :
        using TBase = ALoggerBase<_ThrSafe, TString>;
        using TDef = ALoggerTxtBaseDefaults<_TChar>;

    public :
        /** Default constructor
         *
         * \param[in] local_time Local time will be used for timestamp. By default it is true
         */
        ALoggerTxtBase(bool local_time = true);

        /** Add level descriptor
         *
         * \param[in] level Level identifier
         * \param[in] name Level descriptor
         *
         * \return Current instance reference
         */
        ALoggerTxtBase& addLevelDescr(size_t level, TString name)     { _levelsMap[level] = std::forward<TString>(name); return *this; }

        /** Return levels map */
        const TlevelsMap& levelsMap() const                           { return _levelsMap; }

        /** Output the text message arguments
        *
        * If a task is active, message will be logged. If no task is active, message will be output
        * only if logger level is enabled.
        *
        * Message will be output with the current timestamp.
        *
        * \tparam T Message elements types.
        * \warning Each type must be able to to be used as argument for
        * std::basic_stringstream<TChar>::operator<<(std::forward<T>(args)) call
        *
        * \param[in] level Level identifier
        * \param[in] args Arguments
        *
        * \return Current instance reference
        */
        template<typename... T>
        ALoggerTxtBase& addString(std::size_t level, T&&... args);

        /** Output the text message arguments
        *
        * If a task is active, message will be logged. If no task is active, message will be output
        * only if logger level is enabled.
        *
        * \tparam T Message elements types.
        * \warning Each type must be able to to be used as argument for
        * std::basic_stringstream<TChar>::operator<<(std::forward<T>(args)) call
        *
        * \param[in] time
        * \param[in] level Level identifier
        * \param[in] args Arguments
        *
        * \return Current instance reference
        */
        template<typename... T>
        ALoggerTxtBase& addString(std::chrono::system_clock::time_point time, std::size_t level, T&&... args);

        /** Set timestamp to string format
         *
         * \param[in] output_format std::put_time timestamp format
         *
         * \return Current instance reference
         */
        ALoggerTxtBase& setDateOutputFormat(TString&& output_format)  { _outputFormat = std::forward<TString>(output_format); return *this; }

        /** Default text before level descriptor
         *
         * \param[in] level_prefix Level prefix
         *
         * \return Current instance reference
         */
        ALoggerTxtBase& setLevelPrefix(TString&& level_prefix)        { _levelPrefix  = std::forward<TString>(level_prefix);  return *this; }

        /** Default text after level descriptor
         *
         * \param[in] level_postfix Level postfix
         *
         * \return Current instance reference
         */
        ALoggerTxtBase& setLevelPostfix(TString&& level_postfix)      { _levelPostfix = std::forward<TString>(level_postfix); return *this; }

        /** Default space text
         *
         * \param[in] space Space between different elements
         *
         * \return Current instance reference
         */
        ALoggerTxtBase& setSpace(TString&& space)                     { _space         = std::forward<TString>(space);         return *this; }

        /** Output the text message arguments
        *
        * If a task is active, message will be logged. If no task is active, message will be output
        * only if logger level is enabled.
        *
        * Message will be output with the current timestamp.
        *
        * \tparam T Message elements types.
        * \warning Each type must be able to to be used as argument for
        * std::basic_stringstream<TChar>::operator<<(std::forward<T>(args)) call
        *
        * \param[in] level Level identifier
        * \param[in] args Arguments
        *
        * \return Current instance reference
        */
        template<typename... T>
        ALoggerTxtBase& operator() (std::size_t level, T&&... args)    { return addString(level, args...); }

        /** Output the text message arguments
        *
        * If a task is active, message will be logged. If no task is active, message will be output
        * only if logger level is enabled.
        *
        * \tparam T Message elements types.
        * \warning Each type must be able to to be used as argument for
        * std::basic_stringstream<TChar>::operator<<(std::forward<T>(args)) call
        *
        * \param[in] time
        * \param[in] level Level identifier
        * \param[in] args Arguments
        *
        * \return Current instance reference
        */
        template<typename... T>
        ALoggerTxtBase& operator() (std::chrono::system_clock::time_point time, std::size_t level, T&&... args)    { return addString(time, level, args...); }

    protected:
        /** Decorate string
         *
         * This function decorates string by using prefixe, postfix, timestamp format and space string. It is intented
         * to be called by children classes.
         *
         * \param[in] level Level identifier
         * \param[in] time Message timestamp
         * \param[in] data Message string
         *
         * \return Prepared string
         */
        TString prepareString(std::size_t level, std::chrono::system_clock::time_point time, const TString& data) const;

    private:
        TlevelsMap _levelsMap;
        std::function<std::tm* (const std::time_t*)> _timeConverter;
        TString _outputFormat;
        TString _levelPrefix;
        TString _levelPostfix;
        TString _space;

    };

    template<bool _ThrSafe, typename _TChar>
    ALoggerTxtBase<_ThrSafe, _TChar>::ALoggerTxtBase(bool local_time):
        _timeConverter(local_time ? std::localtime : std::gmtime),
        _outputFormat{ TDef::_defaultOutputFormat },
        _levelPrefix{  TDef::_defaultLevelPrefix  },
        _levelPostfix{ TDef::_defaultLevelPostfix },
        _space{         TDef::_defaultSpace         }
        { }

    template<bool _ThrSafe, typename _TChar>
    template<typename... T>
    ALoggerTxtBase<_ThrSafe, _TChar>& ALoggerTxtBase<_ThrSafe, _TChar>::addString(std::chrono::system_clock::time_point time, std::size_t level, T&&... args)
    {
        if (!TBase::taskOrToBeAdded(level))
            return *this;
        std::basic_stringstream<_TChar> stream;
        (stream << ... << std::forward<T>(args));
        TBase::addToLog(level, stream.str(), time);
        return *this;
    }

    template<bool _ThrSafe, typename _TChar>
    template<typename... T>
    ALoggerTxtBase<_ThrSafe, _TChar>& ALoggerTxtBase<_ThrSafe, _TChar>::addString(std::size_t level, T&&... args)
    {
        return addString(std::chrono::system_clock::now(), level, args...);
    }

    template<bool _ThrSafe, typename _TChar>
    typename ALoggerTxtBase<_ThrSafe, _TChar>::TString ALoggerTxtBase<_ThrSafe, _TChar>::prepareString(std::size_t level, std::chrono::system_clock::time_point time, const TString& data) const
    {
        const auto level_it = _levelsMap.find(level);
        TString str;

        assert(level_it != _levelsMap.cend());

        std::time_t time_moment = std::chrono::system_clock::to_time_t(time);
        std::basic_stringstream<_TChar> sstr;
        sstr << std::put_time(_timeConverter(&time_moment), _outputFormat.c_str());
        str = sstr.str() + _space + _levelPrefix + level_it->second + _levelPostfix + _space + data;

        return str;
    }

    template<> inline std::string ALoggerTxtBaseDefaults<char>::_defaultOutputFormat{ "%F %T" };
    template<> inline std::string ALoggerTxtBaseDefaults<char>::_defaultLevelPrefix { "[" };
    template<> inline std::string ALoggerTxtBaseDefaults<char>::_defaultLevelPostfix{ "]" };
    template<> inline std::string ALoggerTxtBaseDefaults<char>::_defaultSpace       { " " };

    template<> inline std::wstring ALoggerTxtBaseDefaults<wchar_t>::_defaultOutputFormat{ L"%F %T" };
    template<> inline std::wstring ALoggerTxtBaseDefaults<wchar_t>::_defaultLevelPrefix { L"[" };
    template<> inline std::wstring ALoggerTxtBaseDefaults<wchar_t>::_defaultLevelPostfix{ L"]" };
    template<> inline std::wstring ALoggerTxtBaseDefaults<wchar_t>::_defaultSpace       { L" " };

} // namespace ALogger

#endif  // _AVN_LOGGER_TXT_BASE_H_
