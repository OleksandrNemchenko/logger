// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*! \file logger_txt_base.h
 * \brief LoggerTxtBase class implements base text messages logging functionality.
 *
 * #ALogger::LoggerTxtBase class is the #ALogger::ALoggerBase child that implements text messages logging. It introduces logger level
 * description map that will be used for logger message prefix. You have to specify multithreading security mode (see
 * #ALogger::ALoggerBase class description) and character type. It could be char, wchar_t etc.
 *
 * You can tune output format by specifying logger message maker by #ALogger::ALoggerBase::setStringMaker call. This function
 * sets #ALogger::ALoggerBase::TStringMaker message maker function. By default #ALogger::defaultStringMakerChar,
 * #ALogger::defaultStringMakerWChar etc. implementations are used.
 *
 * \warning If you implement not supported character type, no text decoration will be used. Set string maker in your child
 * class by #ALogger::ALoggerBase::setStringMaker call.
 *
 */

#ifndef _AVN_LOGGER_TXT_BASE_H_
#define _AVN_LOGGER_TXT_BASE_H_

#include <ctime>
#include <iomanip>
#include <functional>
#include <sstream>
#include <type_traits>

#include <avn/logger/logger_base.h>

namespace ALogger {

    /** Unspecialized template to use std::basic_stringstream<_TChar>::operator<< call
     *
     * \note You can specialize this function for your type
     *
     * \tparam _TChar String stream std::basic_stringstream::char_type type
     * \tparam T Argument type
     * \param[in] stream String stream
     * \param[in] arg Argument
     */
    template<typename _TChar, typename T>
    inline void ToStrStream(std::basic_stringstream<_TChar>& stream, T&& arg) noexcept
    {
        using TRaw = std::remove_cv_t<std::remove_reference<T>>;

#ifdef ALOGGER_SUPPORT_QT
        if constexpr (std::is_same_v<T, QString> || std::is_same_v<T, const QString> || std::is_same_v<T, const QString&> || std::is_same_v<T, QString&> )
        {
            if constexpr (std::is_same_v<_TChar, char>)
                stream << arg.toStdString();
            else
                stream << arg.toStdWString();
        }
        else
#endif
            stream << std::forward<T>(arg);
    }

    /** Base class for text loggers
     *
     * \tparam _TChar Character data type. Can be char, wchar_t etc.
     */
    template<typename _TChar>
    class LoggerTxtBase : public LoggerBase<std::basic_string<_TChar>> {
    public :
        /** Character type for text logger messages */
        using TChar = _TChar;

        static_assert(std::is_same_v<TChar,char> || std::is_same_v<TChar,wchar_t>, "Only char and wchar_t are currently supported");

        /** String type for text logger messages */
        using TString = std::basic_string<TChar>;

        /** String view type for text logger messages */
        using TStringView = std::basic_string_view<TChar>;

        /** Levels mapping type */
        using TlevelsMap = std::map<size_t, TString>;

        /** Function type to make string
         *
         * This function type is used to make string by using level title, timestamp and data. It is used as
         * #ALogger::LoggerTxtBase::setStringMaker function parameter to set default string maker.
         *
         * \param[in] level Level descriptor
         * \param[in] time Message timestamp
         * \param[in] data Message string
         *
         * \return Prepared string
         */
        using TStringMaker = std::function<TString (const LoggerTxtBase& instance, const TString& level, std::chrono::system_clock::time_point time, TString&& data)>;

    private :
        using TBase = LoggerBase<TString>;

    public :
        /** Default constructor
         *
         * \param[in] thrSafe Boolean flag that has to be true if thread security must be on. By default it is true
         * \param[in] localTime Local time will be used for timestamp. By default it is true
         */
        LoggerTxtBase(bool thSafe = true, bool localTime = true) noexcept;

        /** Add level descriptor
         *
         * \param[in] level Level identifier
         * \param[in] name Level descriptor
         *
         * \return Current instance reference
         */
        LoggerTxtBase& AddLevelDescr(size_t level, TStringView name) noexcept  { _levelsMap[level] = name; return *this; }

        /** Return levels map */
        const TlevelsMap& LevelsMap() const noexcept                            { return _levelsMap; }

        /** Output the text message arguments
        *
        * If a task is active, message will be logged. If no task is active, message will be output
        * only if logger level is enabled.
        *
        * Message will be output with the current timestamp.
        *
        * \tparam T Message elements types.
        * \warning Each type must be able to to be used as argument for
        * std::basic_stringstream<TChar>::operator<<(std::forward<T>(args)) call. If not, specialize #ALogger::ToStrStream
        * function
        *
        * \param[in] level Level identifier
        * \param[in] args Arguments
        *
        * \return Current instance reference
        */
        template<typename... T>
        LoggerTxtBase& AddString(std::size_t level, T&&... args) noexcept;

        /** Output the text message arguments
        *
        * If a task is active, message will be logged. If no task is active, message will be output
        * only if logger level is enabled.
        *
        * Message will be output with the current timestamp.
        *
        * \tparam T Message elements types.
        * \warning Each type must be able to to be used as argument for
        * std::basic_stringstream<TChar>::operator<<(std::forward<T>(args)) call. If not, specialize #ALogger::ToStrStream
        * function
        *
        * \param[in] level Level identifier
        * \param[in] args Arguments
        *
        * \return Current instance reference
        */
        template<typename... T>
        LoggerTxtBase& operator() (std::size_t level, T&&... args) noexcept    { return AddString(level, std::forward<T>(args)...); }

        /** Set the associated locale of the stream to the given one
         *
         * \param[in] loc New locale to associate the stream to
         */
        virtual void Imbue(const std::locale& loc) noexcept { (void) loc;}

        /** Output timestamp
         *
         * This function outputs current time. It is intentent to be called by children classes.
         *
         * \param[in] time Timestamp
         *
         * \return String with timestamp
         */
        TString OutputTime(std::chrono::system_clock::time_point time) const noexcept;

        /** Set implementation for string maker
         *
         * Default string maker is initialized during class initialization. You can replace it in your child class.
         *
         * \param[in] stringMaker String maker implementation
         * \return Current instance reference
         */
        LoggerTxtBase& SetStringMaker(TStringMaker stringMaker) noexcept { _stringMaker = stringMaker; return *this; }

        void ChronoTimeToTm(std::chrono::system_clock::time_point time, tm& timeBuffer) const noexcept
        {
            time_t timeMoment{ std::chrono::system_clock::to_time_t(time) };

#if defined(_MSC_VER)
            _timeConverter(&timeBuffer, &timeMoment);
#else
            tm* timeBufPtr = _timeConverter(&timeMoment);
            timeBuffer = *timeBufPtr;
#endif // _MSC_VER
        }

    private:
        TlevelsMap _levelsMap;

#if defined(_MSC_VER)
        std::function<errno_t (tm*, const std::time_t*)> _timeConverter;
#else
        std::function<std::tm* (const std::time_t*)> _timeConverter;
#endif // _MSC_VER
        TStringMaker _stringMaker;
    };

    inline std::string DefaultStringMakerChar(const LoggerTxtBase<char>& instance, const std::string& level, std::chrono::system_clock::time_point time, const std::string& data) noexcept
    {
        using namespace std;
        basic_stringstream<char> sstr;

        sstr << instance.OutputTime(time) << " ["s << level << "] "s << data;

        return sstr.str();
    }

    inline std::wstring DefaultStringMakerWChar(const LoggerTxtBase<wchar_t>& instance, const std::wstring& level, std::chrono::system_clock::time_point time, const std::wstring& data) noexcept
    {
        using namespace std;
        basic_stringstream<wchar_t> sstr;

        sstr << instance.OutputTime(time) << L" ["s << level << L"] "s << data;

        return sstr.str();
    }

    template<typename _TChar>
    LoggerTxtBase<_TChar>::LoggerTxtBase(bool thrSafe, bool localTime) noexcept:
        TBase(thrSafe),
        _timeConverter(localTime ?
#if defined(_MSC_VER)
        localtime_s : gmtime_s
#else
        localtime : gmtime
#endif  // _MSC_VER
        )
    {
        (void)localTime;
        using namespace std;
        if constexpr (is_same_v<_TChar, char>)
            _stringMaker = DefaultStringMakerChar;
        else if constexpr (is_same_v<_TChar, wchar_t>)
            _stringMaker = DefaultStringMakerChar;
        else
        {
            assert(false && "unsupported type");
        }
    }

    template<typename _TChar>
    typename LoggerTxtBase<_TChar>::TString LoggerTxtBase<_TChar>::OutputTime(std::chrono::system_clock::time_point time) const noexcept
    {
        using namespace std;
        tm timeBuf;

        ChronoTimeToTm(time, timeBuf);

        if constexpr (is_same_v<_TChar, char>)
        {
            basic_stringstream<char> sstr;
            sstr << put_time(&timeBuf, "%F %T");
            return sstr.str();
        }
        else if constexpr (is_same_v<_TChar, wchar_t>)
        {
            basic_stringstream<wchar_t> sstr;
            sstr << put_time(&timeBuf, L"%F %T");
            return sstr.str();
        }
        else
        {
            assert(false && "unsupported character type");
            return TString{};
        }
    }

    template<typename _TChar>
    template<typename... T>
    LoggerTxtBase<_TChar>& LoggerTxtBase<_TChar>::AddString(std::size_t level, T&&... args) noexcept
    {
        const auto level_it{ _levelsMap.find(level) };
        assert(level_it != _levelsMap.cend());

        if (!TBase::CanBeAddedToLog(level))
            return *this;

        std::chrono::system_clock::time_point time = std::chrono::system_clock::now();

        std::basic_stringstream<_TChar> stream;
        (ToStrStream(stream, std::forward<T>(args)), ...);

        TString str = _stringMaker(*this, level_it->second, time, stream.str());
        
        TBase::AddToLog(level, str);

        return *this;
    }

} // namespace ALogger

#endif  // _AVN_LOGGER_TXT_BASE_H_
