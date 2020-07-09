// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*! \file logger_txt_group.h
 * \brief LoggerTxtGroup class can be used as #ALogger::ALoggerTxtBase children classes
 *
 * This class is #ALogger::ALoggerGroup class child. It is the templated container for different loggers that can be used simultaneously, i. e. you output
 * the same logger message to different targets. For example, you can output the same message for console, disk, network etc.
 * You can enable different logger levels for each target.
 *
 * \warning All loggers inside one #ALogger::LoggerTxtGroup must have the same \a TLogData logger message data type.
 *
 * Here is an example of its usage :
 *
 * \code
constexpr auto WARNING;                                 // ALogger level

ALogger::LoggerTxtGroup<                                // Loggers group
        ALogger::ALoggerTxtCOut<true, wchar_t>,          // std::cout target
        ALogger::ALoggerTxtFile<true, wchar_t>> _log;    // Text file target

const std::locale utf8_locale = locale(locale(), new codecvt_utf8<wchar_t>());

_log.ALogger<1>().imbue(utf8_locale);                  // Sets locale for text file

_log.enableLevel(WARNING);                            // Enables WARNING levels
_log.addString(WARNING, L"Warning message!");         // Outputs message

 * \endcode
 *
 * In this example WARNING level is enabled and some warning message is sent simultaneously to the file and std::cout.
 *
 */

#ifndef _AVN_LOGGER_LOGGER_TXT_GROUP_H
#define _AVN_LOGGER_LOGGER_TXT_GROUP_H

#include <avn/logger/logger_txt_base.h>
#include <avn/logger/logger_group.h>

namespace ALogger {

    /** #ALogger::ALoggerTxtBase children classes container
     *
     * \tparam _TLogger #ALogger::ALoggerTxtBase derived classes.
     * \warning All template types must have the same TString type
     */
    template< typename... _TLogger >
    class LoggerTxtGroup : public LoggerGroup<_TLogger...> {
    private:
        using TBase = LoggerGroup<_TLogger...>;
        using TArray = std::tuple<_TLogger...>;

    public :
        /** Character type */
        using TChar = typename std::tuple_element_t<0, TArray>::TChar;

        /** String type */
        using TString = typename std::tuple_element_t<0, TArray>::TString;

        static_assert((std::is_base_of_v<LoggerTxtBase<TChar>, _TLogger> && ...), "Template parameter must be the LoggerTxtBase child class");

        /** Levels map */
        using TlevelsMap = std::map<size_t, TString>;

        /** Add level description
         *
         * This function calls #ALogger::ALoggerTxtBase::addLevelDescr for each container element
         *
         * \param[in] level Level identifier
         * \param[in] name Level description
         */
        void AddLevelDescr(size_t level, TString name) noexcept;

        /** Output the text message arguments for all container elements simultaneously
        *
        * This function calls #ALogger::ALoggerTxtBase::addString for each container element.
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
        */
        template<typename... T>
        void AddString(std::size_t level, const T&... args) noexcept;

        /** Output the text message arguments for all container elements simultaneously
        *
        * This function calls #ALogger::ALoggerTxtBase::addString for each container element.
        *
        * If a task is active, message will be logged. If no task is active, message will be output
        * only if logger level is enabled.
        *
        * \tparam T Message elements types.
        * \warning Each type must be able to to be used as argument for
        * std::basic_stringstream<TChar>::operator<<(std::forward<T>(args)) call. If not, specialize #ALogger::ToStrStream
        * function
        *
        * \param[in] time Message timestamp
        * \param[in] level Level identifier
        * \param[in] args Arguments
       */
        template<typename... T>
        void AddString(std::chrono::system_clock::time_point time, std::size_t level, const T&... args) noexcept;

        /** Output the text message arguments for all container elements simultaneously
        *
        * This function calls #ALogger::ALoggerTxtBase::addString for each container element.
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
        */
        template<typename... T>
        void operator() (std::size_t level, const T&... args) noexcept    { AddString(level, args...); }

        /** Output the text message arguments for all container elements simultaneously
        *
        * This function calls #ALogger::ALoggerTxtBase::addString for each container element.
        *
        * If a task is active, message will be logged. If no task is active, message will be output
        * only if logger level is enabled.
        *
        * \tparam T Message elements types.
        * \warning Each type must be able to to be used as argument for
        * std::basic_stringstream<TChar>::operator<<(std::forward<T>(args)) call. If not, specialize #ALogger::ToStrStream
        * function
        *
        * \param[in] time Message timestamp
        * \param[in] level Level identifier
        * \param[in] args Arguments
        */
        template<typename... T>
        void operator() (std::chrono::system_clock::time_point time, std::size_t level, const T&... args) noexcept    { AddString(time, level, args...); }

        /** Set the associated locale of the stream to the given one
         *
         * \param[in] loc New locale to associate the stream to
         */
        void Imbue(const std::locale& loc) noexcept;

    };  // class LoggerTxtGroup

    template< typename... _TLogger >
    void LoggerTxtGroup<_TLogger...>::AddLevelDescr(size_t level, TString name) noexcept
    {
        std::apply([level,name] (auto&... logger) { (logger.AddLevelDescr(level, name), ...); }, TBase::_loggers);
    }

    template< typename... _TLogger >
    template<typename... T>
    void LoggerTxtGroup<_TLogger...>::AddString(std::size_t level, const T&... args) noexcept
    {
        std::apply([level,args...] (auto&... logger) { (logger.AddString(level, args...), ...); }, TBase::_loggers);
    }

    template< typename... _TLogger >
    template<typename... T>
    void LoggerTxtGroup<_TLogger...>::AddString(std::chrono::system_clock::time_point time, std::size_t level, const T&... args) noexcept
    {
        std::apply([time,level,args...] (auto&... logger) { (logger.AddString(time, level, args...), ...); }, TBase::_loggers);
    }

    template< typename... _TLogger >
    void LoggerTxtGroup<_TLogger...>::Imbue(const std::locale& loc) noexcept
    {
        std::apply([loc] (auto&... logger) { (logger.Imbue(loc), ...); }, TBase::_loggers);
    }

}   // namespace ALogger

#endif // _AVN_LOGGER_LOGGER_GROUP_H
