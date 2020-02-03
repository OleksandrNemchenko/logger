// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*! \file logger_txt_cout.h
 * \brief CLoggerTxtCOut class implements text logging to the std::cout stream.
 *
 * As #Logger::CLoggerBase child this class has features listed below :
 * - multithreading or single thread mode.
 * - enable or disable logger levels. If current output message has level that is enabled now, it will be output. Also it
 * is possible to output regardless of current logger level by using #ForceAddToLog call.
 * - add logger tasks and automatically finish them.
 *
 * In multithread mode all output calls are protected by thread. However, it is redundant for single thread mode. This
 * is why single thread mode is introduced.
 *
 * Logger levels separate all messaged to "enabled" and "disabled" messages. During logger work (usually at the beginning)
 * you enable some logger levels. After you output messages with logger level. Like this you can disable, say, debug
 * messages for normal operation mode.
 *
 * Its usage is obvious :
 * \code

    constexpr auto WARNING = 0;     // WARNING identifier
 
    Logger::CLoggerTxtCOut<false, wchar_t> wlog;    // Wide string logger
    Logger::CLoggerTxtCOut<true, char> logger;         // ASCII string logger

    wlog.AddLevelDescr( WARNING, L"WARNING" );
    wlog.EnableLevel( WARNING );
    wlog.AddString( WARNING, L"This is test wstring : integer = ", 10 );

    logger.AddLevelDescr( WARNING, "TEST-0" );
    logger.EnableLevel( WARNING );
    logger.AddString( WARNING, "This is test string : integer = ", 10 );

 * \endcode
 */

#ifndef _AVN_LOGGER_TXT_COUT_H_
#define _AVN_LOGGER_TXT_COUT_H_

#include <iostream>

#include <avn/logger/logger_txt_base.h>

namespace Logger {

/** Output console stream class
 * 
 * This class instantiates output stream for different _TChar type. If you use non-standart _TChar, you have to implement
 * OutStream for this type
 * 
 * \tparam _TChar Character type
 */
template<typename _TChar>
class CLoggerTxtCOutStream {
protected:
    
    /** Output stream for given _TChar
     * 
     * \return Output stream 
     */
    std::basic_ostream<_TChar> &OutStream();
};

/** Text stream logger message
 * 
 * \tparam _ThrSafe Thread security mode. If true, multithread mode will be used. Otherwise single ther will be activated.
 * @tparam _TChar Character type. Can be char, wchar_t etc.
 */
template<bool _ThrSafe, typename _TChar>
class CLoggerTxtCOut : public CLoggerTxtBase<_ThrSafe, _TChar>, public CLoggerTxtCOutStream<_TChar> {
public:
    /** Current thread security mode. If true, multithread mode will be used. Otherwise single ther will be activated */
    constexpr static bool ThrSafe{ _ThrSafe };

    /** Character type for text logger messages */
    using TChar = _TChar;

    /** String type for text logger messages */
    using TString = std::basic_string<_TChar>;

    /** String stream base class */
    using TStream = CLoggerTxtCOutStream<_TChar>;

    /** Default constructor with time zone selector
     *
     * \param local_time Local time or GMT will be used as time zone. Loca time is selected by default
     */
    CLoggerTxtCOut( bool local_time = true ): CLoggerTxtBase<_ThrSafe, _TChar>(local_time)    {}

private:

    bool OutData( std::size_t level, std::chrono::system_clock::time_point time, TString &&data ) override final;
};

template<bool _ThrSafe, typename _TChar>
bool CLoggerTxtCOut<_ThrSafe, _TChar>::OutData( std::size_t level, std::chrono::system_clock::time_point time, TString &&data ){

    TStream::OutStream() << CLoggerTxtBase<_ThrSafe, _TChar>::PrepareString( level, time, std::move(data) ) << std::endl;

    return true;
}

template<> inline std::ostream  &CLoggerTxtCOutStream<char>::OutStream()    { return std::cout; }
template<> inline std::wostream &CLoggerTxtCOutStream<wchar_t>::OutStream() { return std::wcout; }

} // namespace Logger

#endif  // _AVN_LOGGER_TXT_COUT_H_
