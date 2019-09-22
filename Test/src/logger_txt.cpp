
#include <functional>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

#include <tests.h>
#include <avn/logger/logger_txt.h>

size_t test_logger_txt( void ){

    using namespace std;

    Logger::CLoggerWTxt log( L"/tmp/test.txt"s, false );
    const std::locale utf8_locale = locale(locale(), new codecvt_utf8<wchar_t>());

    log.Imbue( utf8_locale );
    log.AddLevelDescr( 0, L"TEST-0" );
    log.OnLevel( 0 );
    log.AddToLog( 0, { L"test"s, L"string"s, L"ором мы в понедельник говорили"s } );

    return 0;
}