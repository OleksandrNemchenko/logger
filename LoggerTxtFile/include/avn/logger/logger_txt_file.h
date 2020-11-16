// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*! \file logger_txt_file.h
 * \brief LoggerTxtFile class implements text logging to a file.
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
 * #ALogger::LoggerTxtFile usage is obvious :
 *
 * \code

    constexpr auto WARNING = 0;     // WARNING identifier

    ALogger::LoggerTxtFile<true, wchar_t> logger(L"/tmp/test.txt"s);
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
     * \tparam _TChar Character type. Can be char, wchar_t etc.
     */
    template<typename _TChar>
    class LoggerTxtFile : public LoggerTxtBase<_TChar> {
    public:
        /** Character type for text logger messages */
        using TChar = LoggerTxtBase<_TChar>::TChar;

        /** String type for text logger messages */
        using TString = LoggerTxtBase<_TChar>::TString;
        
        /** String view type for text logger messages */
        using TStringView = LoggerTxtBase<_TChar>::TStringView;

        /** File stream type */
        using TStream = std::basic_ofstream<_TChar>;

        /** Base logger */
        using TBase = LoggerTxtBase<_TChar>;

        /** Default constructor
         *
         * \param[in] thrSafe Boolean flag that has to be true if thread security must be on. By default it is true
         * \param[in] local_time Use local time instead of GMT one. True by default
         */
        LoggerTxtFile(bool thrSafe = true, bool local_time = true) noexcept : LoggerTxtBase<_TChar>(thrSafe, local_time), _flushAlways(false)    {}

        /** Constructor with output file configuration
         *
         * #openFile is called after object construction
         *
         * \param[in] filename Output file name and path
         * \param[in] mode File mode as std::ios_base::openmode mask. std::ios_base::out by default
         * \param[in] local_time  Use local time instead of GMT one. True by default
         */
        LoggerTxtFile(const std::filesystem::path& filename, std::ios_base::openmode mode = std::ios_base::out, bool local_time = true) noexcept :
                LoggerTxtFile(local_time)
        {
            OpenFile(filename, mode);
        }

#ifdef ALOGGER_SUPPORT_QT
        /** Constructor with output file configuration
         *
         * #OpenFile is called after object construction
         *
         * \param[in] filename Output file name and path
         * \param[in] mode File mode as std::ios_base::openmode mask. std::ios_base::out by default
         * \param[in] local_time  Use local time instead of GMT one. True by default
         */
        LoggerTxtFile(const QString& filename, std::ios_base::openmode mode = std::ios_base::out, bool local_time = true) noexcept :
                LoggerTxtFile(local_time)
        {
            OpenFile(filename, mode);
        }
#endif // ALOGGER_SUPPORT_QT

        /** Open file
         *
         * \param[in] filename Output file name and path
         * \param[in] mode File mode as std::ios_base::openmode mask. std::ios_base::out by default
         *
         * \return Current instance reference
         */
        LoggerTxtFile& OpenFile(const std::filesystem::path& filename, std::ios_base::openmode mode = std::ios_base::out) noexcept;

#ifdef ALOGGER_SUPPORT_QT
        /** Open file
         *
         * \param[in] filename Output file name and path
         * \param[in] mode File mode as std::ios_base::openmode mask. std::ios_base::out by default
         *
         * \return Current instance reference
         */
        LoggerTxtFile& OpenFile(const QString& filename, std::ios_base::openmode mode = std::ios_base::out)    { return OpenFile(filename.toStdWString(), mode); }
#endif // ALOGGER_SUPPORT_QT

        /** Close currently opened file
         *
         * \return Current instance reference
         */
        LoggerTxtFile& CloseFile() noexcept   { _fstream.close(); return *this; }

        /** Flush all output messages to the output file
         *
         * \return Current instance reference
         */
        LoggerTxtFile& FlushFile() noexcept                                 { _fstream.flush(); return *this; }

        /** Enable automatic flushing for specific message levels to the output file
         *
         * You can flush \a levels specified instantly hen you add such logger messages
         *
         * \param[in] levels levels list to be flushed instantly
         *
         * \return Current instance reference
         */
        LoggerTxtFile& SetFlushLevels(const TLevels& levels) noexcept       { _flushLevels = levels; _flushAlways = false; return *this; }

        /** Enable or diable automatic flushing for all messages to the output file
         *
         * \param[in] flush_always Enable or disable automatic flushing. True by default
         *
         * \return Current instance reference
         */
        LoggerTxtFile& SetFlushAlways(bool flush_always = true) noexcept    { _flushAlways = flush_always; return *this; }

        /** Set the associated locale of the file stream to the given one
         *
         * \param[in] loc New locale to associate the stream to
         */
        void Imbue(const std::locale& loc) noexcept override                { _fstream.imbue(loc); }

        /** Get output file stream
         *
         * \return Output file stream
         */
        TStream& Stream() noexcept                                          { return _fstream; }

        /** Check that output file is opened
         *
         * \return True if file is opened
         */
        bool IsOpenedFile() const noexcept                                  { return _fstream.is_open(); }

        /** Return current file name path
         *
         * \return Log file path
         */
        std::filesystem::path FilePath() const noexcept                     { return _filename; }

        /** Get output file stream
         *
         * \return Output file stream
         */
        const TStream& Stream() const noexcept                              { return _fstream; }

        /** Get output file stream
         *
         * \return Output file stream
         */
        operator TStream& () noexcept                                       { return Stream(); }

        /** Check that output file is opened
         *
         * \return True if file is opened
         */
        operator bool () const noexcept                                     { return IsOpenedFile(); }

        /** Get output file stream
         *
         * \return Output file stream
         */
        operator const TStream& () const noexcept                           { return Stream(); }

    private:
        bool OutputImpl(const TString& data) noexcept override;

        std::filesystem::path _filename;
        TStream _fstream;
        TLevels _flushLevels;
        bool _flushAlways;
    };

    template<typename _TChar>
    bool LoggerTxtFile<_TChar>::OutputImpl(const TString& data) noexcept
    {
        if (_fstream.is_open()) {
            _fstream << data << std::endl;

            if (_flushAlways)
                _fstream.flush();

            return true;
        }
        else
        {
            assert(false);
            return false;
        }
    }

    template<typename _TChar>
    LoggerTxtFile<_TChar>& LoggerTxtFile<_TChar>::OpenFile(const std::filesystem::path& filename, std::ios_base::openmode mode) noexcept
    {
        _filename = filename;
        _fstream.open(filename, mode);

        if (!IsOpenedFile())
            TBase::DisableLogger();

        return *this;
    }

} // namespace ALogger

#endif  // _AVN_LOGGER_TXT_FILE_H_
