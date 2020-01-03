// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*! \file logger_txt_file.h
 * \brief CLoggerTxtFile class implements text logging to a file.
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
 * You can call \a FlushFile to flush currently added logger message. Also you can call \a SetFlushAlways
 * to enable or disable instant flushing for each new logger message. Or you can specify specific logger levels to be flushed
 * instantly by \a SetFlushLevels call. Flushing feature can be useful in debug mode.
 *
 * #Logger::CLoggerTxtFile usage is obvious :
 *
 * \code

    constexpr auto WARNING = 0;     // WARNING identifier

    Logger::CLoggerTxtFile<true, wchar_t> logger( L"/tmp/test.txt"s );
    const std::locale utf8_locale = locale(locale(), new codecvt_utf8<wchar_t>());

    logger.Imbue( utf8_locale );
    logger.AddLevelDescr( WARNING, L"WARNING" );
    logger.EnableLevel( WARNING );
    logger.AddString( WARNING, L"This is test string : integer = ", 10 );

 * \endcode
 */

#ifndef _AVN_LOGGER_TXT_FILE_H_
#define _AVN_LOGGER_TXT_FILE_H_

#include <filesystem>
#include <fstream>

#include <avn/logger/logger_txt_base.h>

namespace Logger {

/** Text stream logger message
 * 
 * \tparam _ThrSafe Thread security mode. If true, multithread mode will be used. Otherwise single ther will be activated.
 * \tparam _TChar Character type. Can be char, wchar_t etc. 
 */
template<bool _ThrSafe, typename _TChar>
class CLoggerTxtFile : public CLoggerTxtBase<_ThrSafe, _TChar> {
public:
    /** Current thread security mode. If true, multithread mode will be used. Otherwise single ther will be activated */
    constexpr static bool ThrSafe{ _ThrSafe };

    /** Character type for text logger messages */
    using TChar = _TChar;

    /** String type for text logger messages */
    using TString = std::basic_string<_TChar>;

    /** File stream type */
    using TStream = std::basic_ofstream<_TChar>;

    /** Default constructor
     *
     * \param local_time Use local time instead of GMT one. True by default
     */
    CLoggerTxtFile( bool local_time = true ): CLoggerTxtBase<_ThrSafe, _TChar>(local_time), _flush_always( false )    {}

    /** Constructor with output file configuration
     *
     * #OpenFile is called after object construction
     *
     * \param filename Output file name and path
     * \param mode File mode as std::ios_base::openmode mask. std::ios_base::out by default
     * \param local_time  Use local time instead of GMT one. True by default
     */
    CLoggerTxtFile( const std::filesystem::path &filename, std::ios_base::openmode mode = std::ios_base::out, bool local_time = true ) :
            CLoggerTxtFile( local_time )
    { OpenFile( filename, mode ); }

    /** Open file
     *
     * \param filename Output file name and path
     * \param mode File mode as std::ios_base::openmode mask. std::ios_base::out by default
     *
     * \return Current instance reference
     */
    CLoggerTxtFile& OpenFile( const std::filesystem::path &filename, std::ios_base::openmode mode = std::ios_base::out )    { _fstream.open( filename, mode ); return *this; }

    /** Close currently opened file
     *
     * \return Current instance reference
     */
    CLoggerTxtFile& CloseFile()   { _fstream.close(); return *this; }

    /** Flush all output messages to the output file
     *
     * \return Current instance reference
     */
    CLoggerTxtFile& FlushFile()                                 { _fstream.flush(); return *this; }

    /** Enable automatic flushing for specific message levels to the output file
     *
     * You can flush \a levels specified instantly hen you add such logger messages
     *
     * \param levels Levels list to be flushed instantly
     *
     * \return Current instance reference
     */
    CLoggerTxtFile& SetFlushLevels( const TLevels& levels )     { _flush_levels = levels; _flush_always = false; return *this; }

    /** Enable automatic flushing for specific message levels to the output file
     *
     * You can flush \a levels specified instantly hen you add such logger messages
     *
     * \param levels Levels list to be flushed instantly
     *
     * \return Current instance reference
     */
    CLoggerTxtFile& SetFlushLevels( TLevels &&levels )          { _flush_levels = std::move( levels ); _flush_always = false; return *this; }

    /** Enable or diable automatic flushing for all messages to the output file
     *
     * \param flush_always Enable or disable automatic flushing. True by default
     *
     * \return Current instance reference
     */
    CLoggerTxtFile& SetFlushAlways( bool flush_always = true )  { _flush_always = flush_always; return *this; }

    /** Set the associated locale of the file stream to the given one
     *
     * \param loc New locale to associate the stream to
     * \return Current instance reference
     */
    CLoggerTxtFile& Imbue( const std::locale& loc )             { _fstream.imbue( loc ); return *this; }

    /** Get output file stream
     *
     * \return Output file stream
     */
    TStream& Stream()                                           { return _fstream; }

    /** Check that output file is opened
     *
     * \return True if file is opened
     */
    bool IsOpenedFile() const                                   { return _fstream.is_open(); }

    /** Get output file stream
     *
     * \return Output file stream
     */
    const TStream& Stream() const                               { return _fstream; }

    /** Get output file stream
     *
     * \return Output file stream
     */
    operator TStream& ()                                        { return Stream(); }

    /** Check that output file is opened
     *
     * \return True if file is opened
     */
    operator bool () const                                      { return IsOpenedFile(); }

    /** Get output file stream
     *
     * \return Output file stream
     */
    operator const TStream& () const                            { return Stream(); }

private:

    bool OutData( std::size_t level, std::chrono::system_clock::time_point time, TString&& data ) override final;

    TStream _fstream;
    TLevels _flush_levels;
    bool _flush_always;

};

template<bool _ThrSafe, typename _TChar>
bool CLoggerTxtFile<_ThrSafe, _TChar>::OutData( std::size_t level, std::chrono::system_clock::time_point time, TString &&data ){

    assert( _fstream.is_open() );

    if( _fstream.is_open() ){
        _fstream << CLoggerTxtBase<_ThrSafe, _TChar>::PrepareString( level, time, std::move(data) ) << std::endl;

        if( _flush_always || _flush_levels.find(level) != _flush_levels.cend() )
            _fstream.flush();

        return true;
    }

    return false;
}

} // namespace Logger

#endif  // _AVN_LOGGER_TXT_FILE_H_
