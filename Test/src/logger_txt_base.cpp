
#include <iostream>
#include <string>

#include <tests.h>
#include <avn/logger/logger_txt_base.h>
#include <avn/logger/logger_txt_group.h>

using namespace std::string_literals;

namespace
{

class LoggerTxt : public ALogger::LoggerTxtBase<char>
{
public:
    LoggerTxt() : LoggerTxtBase(true)    { SetStringMaker(StringMaker); }
    size_t cntr = 0;

    void SetLogLevel(ALogger::TLevel level) noexcept    { _level = level; }
    bool operator<<(const std::string& str) noexcept    { return AddToLog(_level, str); }

private:
    ALogger::TLevel _level = 0;
    bool OutputImpl(const TLogData& logData) noexcept override
    {
        (void) logData;
        ++cntr;
        return true;
    }

    static TString StringMaker(const ALogger::LoggerTxtBase<char>&instance, const TString& level, const std::chrono::system_clock::time_point& time, const TString& data)
    {
        (void) instance;
        (void) time;
        return level + " " + data;
    }

};

class LoggerTxt2 : public ALogger::LoggerTxtBase<char>
{
public:
    LoggerTxt2() : LoggerTxtBase(false)    { SetStringMaker(StringMaker); }
    size_t cntr = 0;

    void SetLogLevel(ALogger::TLevel level) noexcept    { _level = level; }
    bool operator<<(const std::string& str) noexcept    { return AddToLog(_level, str); }

private:
    ALogger::TLevel _level = 0;
    bool OutputImpl(const TLogData& logData) noexcept override
    {
        (void) logData;
        ++cntr;
        return true;
    }

    static TString StringMaker(const ALogger::LoggerTxtBase<char>&instance, const TString& level, const std::chrono::system_clock::time_point& time, const TString& data)
    {
        (void) instance;
        (void) time;
        return level + "=" + data;
    }

};

size_t _testLogger_txt_base()
{
    size_t errors = 0;
    
    LoggerTxt base;
    base.AddLevelDescr(0, "0").AddLevelDescr(1, "1"s);

    return errors;
}
}

namespace ALogger{
size_t test_txt_base()
{
    size_t res = 0;

    std::cout << "[LOGGER] TxtBase test cases" << std::endl;

    res += _testLogger_txt_base();

    return res;
}
}