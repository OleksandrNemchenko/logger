// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*! \file base_thr_safety.h
 * \brief ALoggerBaseThrSafety class implements different thread modes.
 *
 * #ALogger::ALoggerBase class has \a _ThrSafe boolean template parameter that specifies multithreading mode - to use thread secure
 * algorithm or not. These two modes are implemented by #ALogger::ALoggerBaseThrSafety base class.
 *
 * #ALogger::ALoggerBaseThrSafety class has \a outDataThrSafe function that is used for data output. Its behavior depends on
 * \a _ThrSafe template parameter. If it is true, \a outDataThrSafe function will use std::mutex. Otherwise it will not
 * use any thread safety primitives.
 *
 * Also this class declares \a outData pure virtual function that has to be implemented by children classes. This
 * function is called from \a outDataThrSafe function.
 */

#ifndef _AVN_LOGGER_BASE_THR_SAFETY_H_
#define _AVN_LOGGER_BASE_THR_SAFETY_H_

#include <chrono>
#include <mutex>
#include <thread>

namespace ALogger {

    /** Base class that implements different thread security strategies.
     *
     * #ALogger::ALoggerBaseThrSafety class implements different thread security strategies depend on \a _ThrSafe template
     * parameter. If it is true, this class will use std::mutex. Otherwise it will not use any thread safety primitives.
     *
     * \a _TLogData parameter is used to declare outData pure virtual function that is called from \a outDataThrSafe
     * call.
     *
     * \tparam _ThrSafe If true, \a outDataThrSafe will use std::mutex for multithread security. Otherwise it does not use
     * any thread safety primitives.
     * \tparam _TLogData ALogger data type. It is used to declare \a outData pure virtual function that will output logger data.
     */
    template<bool _ThrSafe, typename _TLogData>
    class ALoggerBaseThrSafety;

    /** Base class that implements thread secure mode. It uses std::mutex to protect logger output.
     *
     * \tparam _TLogData ALogger data type. It is used to declare \a outData pure virtual function that will output logger data.
     */
    template<typename _TLogData>
    class ALoggerBaseThrSafety<true, _TLogData> {
    protected:

        /** ALogger data type */
        using TLogData = _TLogData;

        /** Output data with using thread security mode.
         *
         * This function is used to output logger data with thread security mode.
         *
         * \param[in] level ALogger level. Different security levels has to be implemented by children classes.
         * \param[in] time ALogger event timestamp.
         * \param[in] data Data that has to be output.
         *
         * \return true if data was output successfully or false otherwise.
         */
        bool outDataThrSafe(std::size_t level, std::chrono::system_clock::time_point time, const _TLogData& data)
        {
            std::lock_guard<std::mutex> lock_guard(_outMutex);
            return outData(level, time, data);
        }

        /** Output data.
         *
         * This function is called from #outDataThrSafe to output logger data.
         *
         * \param[in] level ALogger level. Different levels has to be implemented by children classes.
         * \param[in] time ALogger event timestamp.
         * \param[in] data Data that has to be output.
         *
         * \return true if data was output successfully or false otherwise.
         */
        virtual bool outData(std::size_t level, std::chrono::system_clock::time_point time, const _TLogData& data) noexcept = 0;

    private:
        std::mutex _outMutex;
    };

    /** Base class that implement single thread mode.
     *
     * \tparam _TLogData ALogger data type. It is used to declare #outData pure virtual function that will output logger data.
     */
    template<typename _TLogData>
    class ALoggerBaseThrSafety<false, _TLogData> {
    protected:

        /** ALogger data type */
        using TLogData = _TLogData;

        /** Output data with using single thread mode.
         *
         * This function is used to output logger data with single thread mode. Actually it simple calls #outData function.
         *
         * \param[in] level ALogger level. Different levels has to be implemented by children classes.
         * \param[in] time ALogger event timestamp.
         * \param[in] data Data that has to be output.
         *
         * \return true if data was output successfully or false otherwise.
         */
        bool outDataThrSafe(std::size_t level, std::chrono::system_clock::time_point time, const _TLogData& data) noexcept
        {
            return outData(level, time, data);
        }

        /** Output data.
         *
         * This function is called from #outDataThrSafe to output logger data.
         *
         * \param[in] level ALogger level. Different levels has to be implemented by children classes.
         * \param[in] time ALogger event timestamp.
         * \param[in] data Data that has to be output.
         *
         * \return true if data was output successfully or false otherwise.
         */
        virtual bool outData(std::size_t level, std::chrono::system_clock::time_point time, const _TLogData& data) noexcept = 0;
    };

} // namespace ALogger

#endif  // _AVN_LOGGER_BASE_THR_SAFETY_H_
