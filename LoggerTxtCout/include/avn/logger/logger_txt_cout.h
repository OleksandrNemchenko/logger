
#ifndef _AVN_LOGGER_TXT_COUT_H_
#define _AVN_LOGGER_TXT_COUT_H_

#include <iostream>

#include <avn/logger/logger_txt_base.h>

namespace Logger {

template<bool LockThr, typename TChar>
class CLoggerTxtCout : public CLoggerTxtBase<LockThr, TChar> {
public:
    using TBase = CLoggerTxtBase<LockThr, TChar>;
    using TString = std::basic_string<TChar>;

    CLoggerTxtCout( bool local_time = true ): CLoggerTxtBase<LockThr, TChar>(local_time)    {}

private:

    bool OutStrings( std::size_t level, std::chrono::system_clock::time_point time, TString &&data ) override final;

    std::basic_ostream<TChar> &OutStream(void);
};

template<bool LockThr, typename TChar>
bool CLoggerTxtCout<LockThr, TChar>::OutStrings( std::size_t level, std::chrono::system_clock::time_point time, TString &&data ){

    OutStream() << TBase::PrepareString( level, time, std::move(data) ) << std::endl;

    return true;
}

template<> inline std::ostream  &CLoggerTxtCout<true, char>::OutStream(void)    { return std::cout; }
template<> inline std::wostream &CLoggerTxtCout<true, wchar_t>::OutStream(void) { return std::wcout; }

} // namespace Logger

#endif  // _AVN_LOGGER_TXT_COUT_H_
