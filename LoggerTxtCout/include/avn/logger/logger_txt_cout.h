// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef _AVN_LOGGER_TXT_COUT_H_
#define _AVN_LOGGER_TXT_COUT_H_

#include <iostream>

#include <avn/logger/logger_txt_base.h>

namespace Logger {

template<typename TChar>
class CLoggerTxtCOutStream {
protected:
    std::basic_ostream<TChar> &OutStream();
};

template<bool LockThr, typename TChar>
class CLoggerTxtCOut : public CLoggerTxtBase<LockThr, TChar>, public CLoggerTxtCOutStream<TChar> {
public:
    using TBase = CLoggerTxtBase<LockThr, TChar>;
    using TString = std::basic_string<TChar>;
    using TStream = CLoggerTxtCOutStream<TChar>;

    CLoggerTxtCOut( bool local_time = true ): CLoggerTxtBase<LockThr, TChar>(local_time)    {}

private:

    bool OutStrings( std::size_t level, std::chrono::system_clock::time_point time, TString &&data ) override final;
};

template<bool LockThr, typename TChar>
bool CLoggerTxtCOut<LockThr, TChar>::OutStrings( std::size_t level, std::chrono::system_clock::time_point time, TString &&data ){

    TStream::OutStream() << TBase::PrepareString( level, time, std::move(data) ) << std::endl;

    return true;
}

template<> inline std::ostream  &CLoggerTxtCOutStream<char>::OutStream()    { return std::cout; }
template<> inline std::wostream &CLoggerTxtCOutStream<wchar_t>::OutStream() { return std::wcout; }

} // namespace Logger

#endif  // _AVN_LOGGER_TXT_COUT_H_
