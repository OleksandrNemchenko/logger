// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*! \file logger_txt_group.h
 * \brief CLoggerTxtGroup class can be used as #Logger::CLoggerTxtBase children classes
 *
 * This class is #Logger::CLoggerGroup class child. It is the templated container for different loggers that can be used simultaneously, i. e. you output
 * the same logger message to different targets. For example, you can output the same message for console, disk, network etc.
 * You can enable different logger levels for each target.
 *
 * \warning All loggers inside one #Logger::CLoggerTxtGroup must have the same \a TLogData logger message data type.
 *
 * Here is an example of its usage :
 *
 * \code
constexpr auto WARNING;                                 // Logger level

Logger::CLoggerTxtGroup<                                // Loggers group
        Logger::CLoggerTxtCOut<true, wchar_t>,          // std::cout target
        Logger::CLoggerTxtFile<true, wchar_t>> _log;    // Text file target

const std::locale utf8_locale = locale(locale(), new codecvt_utf8<wchar_t>());

_log.Logger<1>().Imbue( utf8_locale );                  // Sets locale for text file

_log.EnableLevel( WARNING );                            // Enables WARNING levels
_log.AddString( WARNING, L"Warning message!" );         // Outputs message

 * \endcode
 *
 * In this example WARNING level is enabled and some warning message is sent simultaneously to the file and std::cout.
 *
 */

#ifndef _AVN_LOGGER_LOGGER_TXT_GROUP_H
#define _AVN_LOGGER_LOGGER_TXT_GROUP_H

#include <avn/logger/logger_group.h>

namespace Logger {

    /** #Logger::CLoggerTxtBase children classes container
     *
     * \tparam _TLogger #Logger::CLoggerTxtBase derived classes.
     * \warning All template types must have the same TString type
     */
    template< typename... _TLogger >
    class CLoggerTxtGroup : public CLoggerGroup<_TLogger...> {
    private:
        using TBase = CLoggerGroup<_TLogger...>;
        using TArray = std::tuple<_TLogger...>;

    public :
        /** Character type */
        using TChar = typename std::tuple_element_t<0, TArray>::TChar;

        /** String type */
        using TString = typename std::tuple_element_t<0, TArray>::TString;

        /** Levels map */
        using TLevelsMap = std::map<size_t, TString>;

        /** Add level description
         *
         * This function calls #Logger::CLoggerTxtBase::AddLevelDescr for each container element
         *
         * \param level Level identifier
         * \param name Level description
         */
        void AddLevelDescr( size_t level, TString name );

        /** Output the text message arguments for all container elements simultaneously
        *
        * This function calls #Logger::CLoggerTxtBase::AddString for each container element.
        *
        * If a task is active, message will be logged. If no task is active, message will be output
        * only if logger level is enabled.
        *
        * Message will be output with the current timestamp.
        *
        * \tparam T Message elements types.
        * \warning Each type must be able to to be used as argument for
        * std::basic_stringstream<TChar>::operator<<( std::forward<T>(args) ) call
        *
        * \param level Level identifier
        * \param args Arguments
        *
        * \return Current instance reference
        */
        template<typename... T>
        void AddString( std::size_t level, const T&... args );

        /** Output the text message arguments for all container elements simultaneously
        *
        * This function calls #Logger::CLoggerTxtBase::AddString for each container element.
        *
        * If a task is active, message will be logged. If no task is active, message will be output
        * only if logger level is enabled.
        *
        * \tparam T Message elements types.
        * \warning Each type must be able to to be used as argument for
        * std::basic_stringstream<TChar>::operator<<( std::forward<T>(args) ) call
        *
        * \param time Message timestamp
        * \param level Level identifier
        * \param args Arguments
        *
        * \return Current instance reference
        */
        template<typename... T>
        void AddString( std::chrono::system_clock::time_point time, std::size_t level, const T&... args );

        /** Set timestamp to string format
         *
         * This function calls #Logger::CLoggerTxtBase::SetDateOutputFormat for each container element.
         *
         * \param output_format std::put_time timestamp format
         *
         * \return Current instance reference
         */
        void SetDateOutputFormat( const TString& output_format );

        /** Default text before level descriptor
         *
         * This function calls #Logger::CLoggerTxtBase::SetLevelPrefix for each container element.
         *
         * \param level_prefix Level prefix
         *
         * \return Current instance reference
         */
        void SetLevelPrefix( const TString& level_prefix );

        /** Default text after level descriptor
         *
         * This function calls #Logger::CLoggerTxtBase::SetLevelPostfix for each container element.
         *
         * \param level_postfix Level postfix
         *
         * \return Current instance reference
         */
        void SetLevelPostfix( const TString& level_postfix );

        /** Default space text
         *
         * \param space Space between different elements
         *
         * This function calls #Logger::CLoggerTxtBase::SetSpace for each container element.
         *
         * \param space Space between different elements
         *
         * \return Current instance reference
         */
        void SetSpace( const TString& space );

        /** Output the text message arguments for all container elements simultaneously
        *
        * This function calls #Logger::CLoggerTxtBase::AddString for each container element.
        *
        * If a task is active, message will be logged. If no task is active, message will be output
        * only if logger level is enabled.
        *
        * Message will be output with the current timestamp.
        *
        * \tparam T Message elements types.
        * \warning Each type must be able to to be used as argument for
        * std::basic_stringstream<TChar>::operator<<( std::forward<T>(args) ) call
        *
        * \param level Level identifier
        * \param args Arguments
        *
        * \return Current instance reference
        */
        template<typename... T>
        void operator() ( std::size_t level, const T&... args )    { AddString( level, args... ); }

        /** Output the text message arguments for all container elements simultaneously
        *
        * This function calls #Logger::CLoggerTxtBase::AddString for each container element.
        *
        * If a task is active, message will be logged. If no task is active, message will be output
        * only if logger level is enabled.
        *
        * \tparam T Message elements types.
        * \warning Each type must be able to to be used as argument for
        * std::basic_stringstream<TChar>::operator<<( std::forward<T>(args) ) call
        *
        * \param time Message timestamp
        * \param level Level identifier
        * \param args Arguments
        *
        * \return Current instance reference
        */
        template<typename... T>
        void operator() ( std::chrono::system_clock::time_point time, std::size_t level, const T&... args )    { AddString( time, level, args... ); }

    };  // class CLoggerTxtGroup

    template< typename... _TLogger >
    void CLoggerTxtGroup<_TLogger...>::AddLevelDescr( size_t level, TString name ) {
        std::apply( [level,name] ( auto&... logger ) { ( logger.AddLevelDescr( level, name ), ... ); }, TBase::_logger );
    }

    template< typename... _TLogger >
    template<typename... T>
    void CLoggerTxtGroup<_TLogger...>::AddString( std::size_t level, const T&... args ) {
        std::apply( [level,args...] ( auto&... logger ) { ( logger.AddString( level, args... ), ... ); }, TBase::_logger );
    }

    template< typename... _TLogger >
    template<typename... T>
    void CLoggerTxtGroup<_TLogger...>::AddString( std::chrono::system_clock::time_point time, std::size_t level, const T&... args ) {
        std::apply( [time,level,args...] ( auto&... logger ) { ( logger.AddString( time, level, args... ), ... ); }, TBase::_logger );
    }

    template< typename... _TLogger >
    void CLoggerTxtGroup<_TLogger...>::SetDateOutputFormat( const TString& output_format ) {
        std::apply( [output_format] ( auto&... logger ) { ( logger.SetDateOutputFormat( output_format ), ... ); }, TBase::_logger );
    }

    template< typename... _TLogger >
    void CLoggerTxtGroup<_TLogger...>::SetLevelPrefix( const TString& level_prefix ) {
        std::apply( [level_prefix] ( auto&... logger ) { ( logger.SetLevelPrefix( level_prefix ), ... ); }, TBase::_logger );
    }

    template< typename... _TLogger >
    void CLoggerTxtGroup<_TLogger...>::SetLevelPostfix( const TString& level_postfix ) {
        std::apply( [level_postfix] ( auto&... logger ) { ( logger.SetLevelPostfix( level_postfix ), ... ); }, TBase::_logger );
    }

    template< typename... _TLogger >
    void CLoggerTxtGroup<_TLogger...>::SetSpace( const TString& space ) {
        std::apply( [space] ( auto&... logger ) { ( logger.SetSpace( space ), ... ); }, TBase::_logger );
    }

}   // namespace Logger

#endif // _AVN_LOGGER_LOGGER_GROUP_H
