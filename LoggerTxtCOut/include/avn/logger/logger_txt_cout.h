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
     * \tparam _TChar Character type. Can be char, wchar_t etc.
     */
    template<bool _ThrSafe, typename _TChar>
    class ALoggerTxtCOut : public ALoggerTxtBase<_ThrSafe, _TChar> {
    public:
        /** Current thread security mode. If true, multithread mode will be used. Otherwise single ther will be activated */
        constexpr static bool ThrSafe{ _ThrSafe };

        /** Character type for text logger messages */
        using TChar = _TChar;

        /** String type for text logger messages */
        using TString = std::basic_string<_TChar>;

        /** Default constructor with time zone selector
         *
         * \param[in] local_time Local time or GMT will be used as time zone. Loca time is selected by default
         */
        ALoggerTxtCOut(bool local_time = true) noexcept : ALoggerTxtBase<_ThrSafe, _TChar>(local_time)    {}

        /** Set the associated locale of the stream to the given one
         *
         * \param[in] loc New locale to associate the stream to
         */
        void imbue(const std::locale& loc) noexcept override            { outStream().imbue(loc); }

    private:
        bool outData(std::size_t level, std::chrono::system_clock::time_point time, const TString& data) noexcept override;
        static std::basic_ostream<_TChar>& outStream() noexcept;
    };

    template<bool _ThrSafe, typename _TChar>
    bool ALoggerTxtCOut<_ThrSafe, _TChar>::outData(std::size_t level, std::chrono::system_clock::time_point time, const TString& data) noexcept
    {
        outStream() << ALoggerTxtBase<_ThrSafe, _TChar>::prepareString(level, time, data) << std::endl;
        return true;
    }

    template<bool _ThrSafe, typename _TChar>
    /* static */ std::basic_ostream<_TChar>& ALoggerTxtCOut<_ThrSafe, _TChar>::outStream() noexcept
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
