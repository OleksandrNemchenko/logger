// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*! \file base_thr_safety.h
 * \brief CLoggerBaseThrSafety class implements different thread modes.
 *
 * #Logger::CLoggerBase class has \a _ThrSafe boolean template parameter that specifies multithreading mode - to use thread secure
 * algorithm or not. These two modes are implemented by #Logger::CLoggerBaseThrSafety base class.
 *
 * #Logger::CLoggerBaseThrSafety class has \a OutDataThrSafe function that is used for data output. Its behavior depends on
 * \a _ThrSafe template parameter. If it is true, \a OutDataThrSafe function will use std::mutex. Otherwise it will not
 * use any thread safety primitives.
 *
 * Also this class declares \a OutData pure virtual function that has to be implemented by children classes. This
 * function is called from \a OutDataThrSafe function.
 */

#ifndef _AVN_LOGGER_BASE_THR_SAFETY_H_
#define _AVN_LOGGER_BASE_THR_SAFETY_H_

#include <chrono>
#include <thread>

namespace Logger {

    /** Base class that implements different thread security strategies.
     *
     * #Logger::CLoggerBaseThrSafety class implements different thread security strategies depend on \a _ThrSafe template
     * parameter. If it is true, this class will use std::mutex. Otherwise it will not use any thread safety primitives.
     *
     * \a _TLogData parameter is used to declare OutData pure virtual function that is called from \a OutDataThrSafe
     * call.
     *
     * \tparam _ThrSafe If true, \a OutDataThrSafe will use std::mutex for multithread security. Otherwise it does not use
     * any thread safety primitives.
     * \tparam _TLogData Logger data type. It is used to declare \a OutData pure virtual function that will output logger data.
     */
    template<bool _ThrSafe, typename _TLogData>
    class CLoggerBaseThrSafety;

    /** Base class that implements thread secure mode. It uses std::mutex to protect logger output.
     *
     * \tparam _TLogData Logger data type. It is used to declare \a OutData pure virtual function that will output logger data.
     */
    template<typename _TLogData>
    class CLoggerBaseThrSafety<true, _TLogData>{
    protected:

        /** Logger data type */
        using TLogData = _TLogData;

        /** Output data with using thread security mode.
         *
         * This function is used to output logger data with thread security mode.
         *
         * \param level Logger level. Different security levels has to be implemented by children classes.
         * \param time Logger event timestamp.
         * \param data Data that has to be output.
         * \return true if data was output successfully or false otherwise.
         */
        bool OutDataThrSafe( std::size_t level, std::chrono::system_clock::time_point time, _TLogData &&data ){
            std::lock_guard<decltype(_out_mutex)> lock_guard( _out_mutex );
            return OutData( level, time, std::move( data ));
        }

        /** Output data.
         *
         * This function is called from #OutDataThrSafe to output logger data.
         *
         * \param level Logger level. Different levels has to be implemented by children classes.
         * \param time Logger event timestamp.
         * \param data Data that has to be output.
         * \return true if data was output successfully or false otherwise.
         */
        virtual bool OutData( std::size_t level, std::chrono::system_clock::time_point time, _TLogData &&data ) = 0;

    private:
        std::mutex _out_mutex;
    };

    /** Base class that implement single thread mode.
     *
     * \tparam _TLogData Logger data type. It is used to declare #OutData pure virtual function that will output logger data.
     */
    template<typename _TLogData>
    class CLoggerBaseThrSafety<false, _TLogData>{
    protected:

        /** Logger data type */
        using TLogData = _TLogData;

        /** Output data with using single thread mode.
         *
         * This function is used to output logger data with single thread mode. Actually it simple calls #OutData function.
         *
         * \param level Logger level. Different levels has to be implemented by children classes.
         * \param time Logger event timestamp.
         * \param data Data that has to be output.
         * \return true if data was output successfully or false otherwise.
         */
        bool OutDataThrSafe( std::size_t level, std::chrono::system_clock::time_point time, _TLogData &&data ){
            return OutData( level, time, std::move( data ));
        }

        /** Output data.
         *
         * This function is called from #OutDataThrSafe to output logger data.
         *
         * \param level Logger level. Different levels has to be implemented by children classes.
         * \param time Logger event timestamp.
         * \param data Data that has to be output.
         * \return true if data was output successfully or false otherwise.
         */
        virtual bool OutData( std::size_t level, std::chrono::system_clock::time_point time, _TLogData &&data ) = 0;
    };

} // namespace Logger

#endif  // _AVN_LOGGER_BASE_THR_SAFETY_H_
