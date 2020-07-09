// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*! \file logger_txt_cout.h
 * \brief ALoggerTxtCOut class implements text logging to the std::cout stream.
 *
 * As #ALogger::ALoggerBase child this class has features listed below :
 * - multithreading or single thread mode.
 * - enable or disable logger levels. If current output message has level that is enabled now, it will be output. Also it
 * is possible to output regardless of current logger level by using #forceAddToLog call.
 * - add logger tasks and automatically finish them.
 *
 * In multithread mode all output calls are protected by thread. However, it is redundant for single thread mode. This
 * is why single thread mode is introduced.
 *
 * ALogger levels separate all messaged to "enabled" and "disabled" messages. During logger work (usually at the beginning)
 * you enable some logger levels. After you output messages with logger level. Like this you can disable, say, debug
 * messages for normal operation mode.
 *
 * Its usage is obvious :
 * \code

    constexpr auto WARNING = 0;     // WARNING identifier
 
    ALogger::ALoggerTxtCOut<false, wchar_t> wlog;    // Wide string logger
    ALogger::ALoggerTxtCOut<true, char> logger;         // ASCII string logger

    wlog.addLevelDescr(WARNING, L"WARNING");
    wlog.enableLevel(WARNING);
    wlog.addString(WARNING, L"This is test wstring : integer = ", 10);

    logger.addLevelDescr(WARNING, "TEST-0");
    logger.enableLevel(WARNING);
    logger.addString(WARNING, "This is test string : integer = ", 10);

 * \endcode
 */

#ifndef _AVN_LOGGER_TXT_COUT_H_
#define _AVN_LOGGER_TXT_COUT_H_

#include <iostream>

#include <avn/logger/logger_txt_base.h>

namespace ALogger {

    /** Text stream logger message
     *
     * \tparam _ThrSafe Thread security mode. If true, multithread mode will be used. Otherwise single ther will be activated.
     * @tparam _TChar Character type. Can be char, wchar_t etc.
     */
    template<typename _TChar>
    class LoggerTxtCOut : public LoggerTxtBase<_TChar> {
    public:
        /** Character type for text logger messages */
        using TChar = LoggerTxtBase<_TChar>::TChar;

        /** String type for text logger messages */
        using TString = LoggerTxtBase<_TChar>::TString;
        
        /** String view type for text logger messages */
        using TStringView = LoggerTxtBase<_TChar>::TStringView;

        /** Default constructor with time zone selector
         *
         * \param[in] thrSafe Boolean flag that has to be true if thread security must be on. By default it is true
         * \param[in] local_time Local time or GMT will be used as time zone. Loca time is selected by default
         */
        LoggerTxtCOut(bool thrSafe = true, bool local_time = true) noexcept : LoggerTxtBase<_TChar>(thrSafe, local_time)    {}

        /** Set the associated locale of the stream to the given one
         *
         * \param[in] loc New locale to associate the stream to
         */
        void Imbue(const std::locale& loc) noexcept override            { OutStream().imbue(loc); }

    private:
        bool OutputImpl(const TString& data) noexcept override;
        static std::basic_ostream<_TChar>& OutStream() noexcept;
    };

    template<typename _TChar>
    bool LoggerTxtCOut<_TChar>::OutputImpl(const TString& data) noexcept
    {
        OutStream() << data << std::endl;
        return true;
    }

    template<typename _TChar>
    /* static */ std::basic_ostream<_TChar>& LoggerTxtCOut<_TChar>::OutStream() noexcept
    {
        static_assert(std::is_same_v<_TChar, char> || std::is_same_v<_TChar, wchar_t>, "Unsupported stream");

        if constexpr (std::is_same_v<_TChar, char>)
            return std::cout;
        else if constexpr (std::is_same_v<_TChar, wchar_t>)
            return std::wcout;
        else {
            assert(false && "unsupported");
            return std::cout;
        }
    }

} // namespace ALogger

#endif  // _AVN_LOGGER_TXT_COUT_H_
