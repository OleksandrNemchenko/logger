
#include <functional>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

#include <tests.h>
#include <avn/logger/logger_txt_cout.h>

size_t test_logger_txt_cout( void ){

    using namespace std;

    std::cout << "START test_txt_cout" << std::endl;

    Logger::CLoggerTxtCout<false, wchar_t> wlog;
    Logger::CLoggerTxtCout<true, char> log;

    wlog.AddLevelDescr( 0, L"TEST-0" );
    wlog.OnLevel( 0 );
    wlog.AddString( 0, L"This is test wstring : integer = ", 10 );

    log.AddLevelDescr( 0, "TEST-0" );
    log.OnLevel( 0 );
    log.AddString( 0, "This is test string : integer = ", 10 );


    return 0;
}