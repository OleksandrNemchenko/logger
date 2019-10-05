
#ifndef _AVN_LOGGER_TXT_COUT_H_
#define _AVN_LOGGER_TXT_COUT_H_

#include <iostream>

#include <avn/logger/logger_txt_base.h>

namespace Logger {

template<typename TChar>
class CLoggerTxtCout : public CLoggerTxtBase<TChar> {
public:
    using TBase = CLoggerTxtBase<TChar>;
    using TString = std::basic_string<TChar>;

    CLoggerTxtCout( bool local_time = true ): CLoggerTxtBase<TChar>(local_time)    {}

private:

    bool OutStrings( std::size_t level, std::chrono::system_clock::time_point time, TString &&data ) override final;

    std::basic_ostream<TChar> &OutStream(void);
};

template<typename TChar>
bool CLoggerTxtCout<TChar>::OutStrings( std::size_t level, std::chrono::system_clock::time_point time, TString &&data ){

    OutStream() << TBase::PrepareString( level, time, std::move(data) ) << std::endl;

    return true;
}

template<> inline std::basic_ostream<char>    &CLoggerTxtCout<char>::OutStream(void)    { return std::cout; }
template<> inline std::basic_ostream<wchar_t> &CLoggerTxtCout<wchar_t>::OutStream(void) { return std::wcout; }

} // namespace Logger

#endif  // _AVN_LOGGER_TXT_COUT_H_
