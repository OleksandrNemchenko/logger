
#include <functional>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

#include <tests.h>
#include <avn/logger/logger_txt_cout.h>

size_t test_logger_txt_cout( void ){

    using namespace std;

    Logger::CLoggerTxtCout<wchar_t> log;

    log.AddLevelDescr( 0, L"TEST-0" );
    log.OnLevel( 0 );
    log.AddString( 0, L"This is test string : integer = ", 10 );


    return 0;
}