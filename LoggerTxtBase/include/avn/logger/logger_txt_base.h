// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*! \file logger_txt_base.h
 * \brief ALoggerTxtBase class implements base text messages logging functionality.
 *
 * #ALogger::ALoggerTxtBase class is the #ALogger::ALoggerBase child that implements text messages logging. It introduces logger level
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

#include <iomanip>
#include <sstream>
#include <type_traits>

#include <avn/logger/logger_base.h>

namespace ALogger {

    /** Unspecialied template to use std::basic_stringstream<_TChar>::operator<< call
     *
     * \note You can specialize this function for your type
     *
     * \tparam _TChar String stream std::basic_stringstream::char_type type
     * \tparam T Argument type
     * \param[in] stream String stream
     * \param[in] arg Argument
     */
    template<typename _TChar, typename T>
    inline void toStrStream(std::basic_stringstream<_TChar> &stream, T &&arg) noexcept { stream << std::forward<T>(arg); }

// Qt Objects
#ifdef QT_VERSION
    /** QString argument for char based text logger
     *
     * \param stream String stream based on char type
     * \param arg QString argument
     */
    inline void toStrStream(std::basic_stringstream<char> &stream, const QString& arg) { stream << arg.toStdString(); }

    /** QString argument for wchar_t based text logger
     *
     * \param stream String stream based on wchar_t type
     * \param arg QString argument
     */
    inline void toStrStream(std::basic_stringstream<wchar_t> &stream, const QString& arg) { stream << arg.toStdWString(); }
#endif // QT_VERSION

    /** Base class for text loggers
     *
     * \tparam _ThrSafe Thread security mode. If true, multithread mode will be used. Otherwise single ther will be activated.
     * \tparam _TChar Character data type. Can be char, wchar_t etc.
     */
    template<bool _ThrSafe, typename _TChar>
    class ALoggerTxtBase : public ALoggerBase<_ThrSafe, std::basic_string<_TChar>> {
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

    public :
        /** Default constructor
         *
         * \param[in] local_time Local time will be used for timestamp. By default it is true
         */
        ALoggerTxtBase(bool local_time = true) noexcept;

        /** Add level descriptor
         *
         * \param[in] level Level identifier
         * \param[in] name Level descriptor
         *
         * \return Current instance reference
         */
        ALoggerTxtBase& addLevelDescr(size_t level, const TString& name) noexcept     { _levelsMap[level] = name; return *this; }

        /** Return levels map */
        const TlevelsMap& levelsMap() const noexcept                           { return _levelsMap; }

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
        ALoggerTxtBase& addString(std::size_t level, T&&... args) noexcept;

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
        ALoggerTxtBase& operator() (std::size_t level, T&&... args) noexcept    { return addString(level, std::forward<T...>(args...)); }

        /** Set the associated locale of the stream to the given one
         *
         * \param[in] loc New locale to associate the stream to
         */
        virtual void imbue(const std::locale& loc) noexcept { }

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
        TString prepareString(std::size_t level, std::chrono::system_clock::time_point time, const TString& data) const noexcept;

        /** Function type to make string
         *
         * This function type is used to make string by using level title, timestamp and data.
         *
         * \param[in] level Level descriptor
         * \param[in] time Message timestamp
         * \param[in] data Message string
         *
         * \return Prepared string
         */
        using TStringMaker = std::function<TString (const TString& level, const std::tm* time, const TString& data)>;

        /** Set child implementation for string maker
         *
         * Default string maker is initialized during class initialization. You can replace it in your child class.
         *
         * \param stringMaker String maker implementation
         * \return Current instance reference
         */
        ALoggerTxtBase& setStringMaker(TStringMaker stringMaker) { _stringMaker = stringMaker; return *this; }

    private:
        TlevelsMap _levelsMap;
        std::function<std::tm* (const std::time_t*)> _timeConverter;
        TStringMaker _stringMaker;

        TStringMaker selectDefaultStringMaker() noexcept;
    };

    template<bool _ThrSafe, typename _TChar>
    ALoggerTxtBase<_ThrSafe, _TChar>::ALoggerTxtBase(bool local_time) noexcept:
            _timeConverter{local_time ? std::localtime : std::gmtime},
            _stringMaker(selectDefaultStringMaker())
    { }

    inline std::string defaultStringMakerChar(const std::string& level, const std::tm* time, const std::string& data) noexcept
    {
        using namespace std;
        basic_stringstream<char> sstr;
        sstr << put_time(time, "%F %T") << " ["s << level << "] "s << data;
        return sstr.str();
    }

    inline std::wstring defaultStringMakerWChar(const std::wstring& level, const std::tm* time, const std::wstring& data) noexcept
    {
        using namespace std;
        basic_stringstream<wchar_t> sstr;
        sstr << put_time(time, L"%F %T") << L" ["s << level << L"] "s << data;
        return sstr.str();
    }

    template<bool _ThrSafe, typename _TChar>
    typename ALoggerTxtBase<_ThrSafe, _TChar>::TStringMaker ALoggerTxtBase<_ThrSafe, _TChar>::selectDefaultStringMaker() noexcept
    {
        if constexpr (std::is_same_v<_TChar, char>)
            return defaultStringMakerChar;
        else if constexpr (std::is_same_v<_TChar, wchar_t>)
            return defaultStringMakerWChar;
        else {
            assert(false && "unsupported character type");
            return [](auto level, auto time, auto data) { return data; };
        }
    }

    template<bool _ThrSafe, typename _TChar>
    template<typename... T>
    ALoggerTxtBase<_ThrSafe, _TChar>& ALoggerTxtBase<_ThrSafe, _TChar>::addString(std::size_t level, T&&... args) noexcept
    {
        std::chrono::system_clock::time_point time = std::chrono::system_clock::now();
        if (!TBase::taskOrToBeAdded(level))
            return *this;
        std::basic_stringstream<_TChar> stream;
        (toStrStream(stream, std::forward<T>(args)), ...);
        TBase::addToLog(level, stream.str(), time);
        return *this;
    }

    template<bool _ThrSafe, typename _TChar>
    typename ALoggerTxtBase<_ThrSafe, _TChar>::TString ALoggerTxtBase<_ThrSafe, _TChar>::prepareString(std::size_t level, std::chrono::system_clock::time_point time, const TString& data) const noexcept
    {
        const auto level_it{ _levelsMap.find(level) };
        assert(level_it != _levelsMap.cend());

        std::time_t time_moment{ std::chrono::system_clock::to_time_t(time) };

        return _stringMaker(level_it->second, _timeConverter(&time_moment), data);
    }

} // namespace ALogger

#endif  // _AVN_LOGGER_TXT_BASE_H_
