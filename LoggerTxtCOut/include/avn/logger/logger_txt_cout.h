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

    /** Output console stream class
     *
     * This class instantiates output stream for different _TChar type. If you use non-standart _TChar, you have to implement
     * OutStream for this type
     *
     * \tparam _TChar Character type
     */
    template<typename _TChar>
    class ALoggerTxtCOutStream {
    protected:

        /** Output stream for given _TChar
         *
         * \return Output stream
         */
        std::basic_ostream<_TChar>& OutStream();
    };

    /** Text stream logger message
     *
     * \tparam _ThrSafe Thread security mode. If true, multithread mode will be used. Otherwise single ther will be activated.
     * @tparam _TChar Character type. Can be char, wchar_t etc.
     */
    template<bool _ThrSafe, typename _TChar>
    class ALoggerTxtCOut : public ALoggerTxtBase<_ThrSafe, _TChar>, public ALoggerTxtCOutStream<_TChar> {
    public:
        /** Current thread security mode. If true, multithread mode will be used. Otherwise single ther will be activated */
        constexpr static bool ThrSafe{ _ThrSafe };

        /** Character type for text logger messages */
        using TChar = _TChar;

        /** String type for text logger messages */
        using TString = std::basic_string<_TChar>;

        /** String stream base class */
        using TStream = ALoggerTxtCOutStream<_TChar>;

        /** Default constructor with time zone selector
         *
         * \param local_time Local time or GMT will be used as time zone. Loca time is selected by default
         */
        ALoggerTxtCOut(bool local_time = true): ALoggerTxtBase<_ThrSafe, _TChar>(local_time)    {}

    private:
        bool outData(std::size_t level, std::chrono::system_clock::time_point time, TString&& data) override final;
    };

    template<bool _ThrSafe, typename _TChar>
    bool ALoggerTxtCOut<_ThrSafe, _TChar>::outData(std::size_t level, std::chrono::system_clock::time_point time, TString&& data)
    {
        TStream::OutStream() << ALoggerTxtBase<_ThrSafe, _TChar>::prepareString(level, time, std::move(data)) << std::endl;
        return true;
    }

    template<> inline std::ostream&  ALoggerTxtCOutStream<char>::OutStream()    { return std::cout;  }
    template<> inline std::wostream& ALoggerTxtCOutStream<wchar_t>::OutStream() { return std::wcout; }

} // namespace ALogger

#endif  // _AVN_LOGGER_TXT_COUT_H_
