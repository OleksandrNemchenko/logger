// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef _AVN_LOGGER_BASE_THR_SAFETY_H_
#define _AVN_LOGGER_BASE_THR_SAFETY_H_

#include <chrono>
#include <thread>

namespace Logger {

    template<bool _ThrSafe, typename _TLogData>
    class CLoggerBaseThrSafety;

    template<typename _TLogData>
    class CLoggerBaseThrSafety<true, _TLogData>{
    protected:
        using TLogData = _TLogData;
        bool OutStringsThrSafe( std::size_t level, std::chrono::system_clock::time_point time, _TLogData &&data ){
            std::lock_guard<decltype(_out_mutex)> lock_guard( _out_mutex );
            return OutStrings( level, time, std::move( data ));
        }
        virtual bool OutStrings( std::size_t level, std::chrono::system_clock::time_point time, _TLogData &&data ) = 0;

    private:
        std::mutex _out_mutex;
    };

    template<typename _TLogData>
    class CLoggerBaseThrSafety<false, _TLogData>{
    protected:
        using TLogData = _TLogData;
        bool OutStringsThrSafe( std::size_t level, std::chrono::system_clock::time_point time, _TLogData &&data ){
            return OutStrings( level, time, std::move( data ));
        }
        virtual bool OutStrings( std::size_t level, std::chrono::system_clock::time_point time, _TLogData &&data ) = 0;
    };

} // namespace Logger

#endif  // _AVN_LOGGER_BASE_THR_SAFETY_H_
