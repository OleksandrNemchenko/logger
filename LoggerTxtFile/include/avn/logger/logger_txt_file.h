// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*! \file logger_txt_file.h
 * \brief ALoggerTxtFile class implements text logging to a file.
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
 * You can call \a flushFile to flush currently added logger message. Also you can call \a SetFlushAlways
 * to enable or disable instant flushing for each new logger message. Or you can specify specific logger levels to be flushed
 * instantly by \a setFlushlevels call. Flushing feature can be useful in debug mode.
 *
 * #ALogger::ALoggerTxtFile usage is obvious :
 *
 * \code

    constexpr auto WARNING = 0;     // WARNING identifier

    ALogger::ALoggerTxtFile<true, wchar_t> logger(L"/tmp/test.txt"s);
    const std::locale utf8_locale = locale(locale(), new codecvt_utf8<wchar_t>());

    logger.imbue(utf8_locale);
    logger.addLevelDescr(WARNING, L"WARNING");
    logger.enableLevel(WARNING);
    logger.addString(WARNING, L"This is test string : integer = ", 10);

 * \endcode
 */

#ifndef _AVN_LOGGER_TXT_FILE_H_
#define _AVN_LOGGER_TXT_FILE_H_

#include <filesystem>
#include <fstream>

#include <avn/logger/logger_txt_base.h>

namespace ALogger {

    /** Text stream logger message
     *
     * \tparam _ThrSafe Thread security mode. If true, multithread mode will be used. Otherwise single ther will be activated.
     * \tparam _TChar Character type. Can be char, wchar_t etc.
     */
    template<bool _ThrSafe, typename _TChar>
    class ALoggerTxtFile : public ALoggerTxtBase<_ThrSafe, _TChar> {
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
         * \param[in] local_time Use local time instead of GMT one. True by default
         */
        ALoggerTxtFile(bool local_time = true) noexcept : ALoggerTxtBase<_ThrSafe, _TChar>(local_time), _flushAlways(false)    {}

        /** Constructor with output file configuration
         *
         * #openFile is called after object construction
         *
         * \param[in] filename Output file name and path
         * \param[in] mode File mode as std::ios_base::openmode mask. std::ios_base::out by default
         * \param[in] local_time  Use local time instead of GMT one. True by default
         */
        ALoggerTxtFile(const std::filesystem::path &filename, std::ios_base::openmode mode = std::ios_base::out, bool local_time = true) noexcept :
                ALoggerTxtFile(local_time)
        {
            openFile(filename, mode);
        }

#ifdef QT_VERSION
        /** Constructor with output file configuration
         *
         * #openFile is called after object construction
         *
         * \param[in] filename Output file name and path
         * \param[in] mode File mode as std::ios_base::openmode mask. std::ios_base::out by default
         * \param[in] local_time  Use local time instead of GMT one. True by default
         */
        ALoggerTxtFile(const QString &filename, std::ios_base::openmode mode = std::ios_base::out, bool local_time = true) noexcept :
                ALoggerTxtFile(local_time)
        {
            openFile(filename, mode);
        }
#endif // QT_VERSION

        /** Open file
         *
         * \param[in] filename Output file name and path
         * \param[in] mode File mode as std::ios_base::openmode mask. std::ios_base::out by default
         *
         * \return Current instance reference
         */
        ALoggerTxtFile& openFile(const std::filesystem::path &filename, std::ios_base::openmode mode = std::ios_base::out) noexcept    { _fstream.open(filename, mode); return *this; }

#ifdef QT_VERSION
        /** Open file
         *
         * \param[in] filename Output file name and path
         * \param[in] mode File mode as std::ios_base::openmode mask. std::ios_base::out by default
         *
         * \return Current instance reference
         */
        ALoggerTxtFile& openFile(const QString &filename, std::ios_base::openmode mode = std::ios_base::out)    { return openFile(filename.toStdWString(), mode); }
#endif // QT_VERSION

        /** Close currently opened file
         *
         * \return Current instance reference
         */
        ALoggerTxtFile& closeFile() noexcept   { _fstream.close(); return *this; }

        /** Flush all output messages to the output file
         *
         * \return Current instance reference
         */
        ALoggerTxtFile& flushFile() noexcept                                 { _fstream.flush(); return *this; }

        /** Enable automatic flushing for specific message levels to the output file
         *
         * You can flush \a levels specified instantly hen you add such logger messages
         *
         * \param[in] levels levels list to be flushed instantly
         *
         * \return Current instance reference
         */
        ALoggerTxtFile& setFlushLevels(const TLevels& levels) noexcept          { _flushLevels = levels; _flushAlways = false; return *this; }

        /** Enable or diable automatic flushing for all messages to the output file
         *
         * \param[in] flush_always Enable or disable automatic flushing. True by default
         *
         * \return Current instance reference
         */
        ALoggerTxtFile& SetFlushAlways(bool flush_always = true) noexcept  { _flushAlways = flush_always; return *this; }

        /** Set the associated locale of the file stream to the given one
         *
         * \param[in] loc New locale to associate the stream to
         */
        void imbue(const std::locale& loc) noexcept override               { _fstream.imbue(loc); }

        /** Get output file stream
         *
         * \return Output file stream
         */
        TStream& stream() noexcept                                         { return _fstream; }

        /** Check that output file is opened
         *
         * \return True if file is opened
         */
        bool IsOpenedFile() const noexcept                                 { return _fstream.is_open(); }

        /** Get output file stream
         *
         * \return Output file stream
         */
        const TStream& stream() const noexcept                             { return _fstream; }

        /** Get output file stream
         *
         * \return Output file stream
         */
        operator TStream& () noexcept                                      { return stream(); }

        /** Check that output file is opened
         *
         * \return True if file is opened
         */
        operator bool () const noexcept                                    { return IsOpenedFile(); }

        /** Get output file stream
         *
         * \return Output file stream
         */
        operator const TStream& () const noexcept                          { return stream(); }

    private:
        bool outData(std::size_t level, std::chrono::system_clock::time_point time, const TString& data) noexcept override;

        TStream _fstream;
        TLevels _flushLevels;
        bool _flushAlways;

    };

    template<bool _ThrSafe, typename _TChar>
    bool ALoggerTxtFile<_ThrSafe, _TChar>::outData(std::size_t level, std::chrono::system_clock::time_point time, const TString& data) noexcept
    {
        assert(_fstream.is_open());

        if (_fstream.is_open()) {
            _fstream << ALoggerTxtBase<_ThrSafe, _TChar>::prepareString(level, time, data) << std::endl;

            if (_flushAlways || _flushLevels.find(level) != _flushLevels.cend())
                _fstream.flush();

            return true;
        }

        return false;
    }

} // namespace ALogger

#endif  // _AVN_LOGGER_TXT_FILE_H_
