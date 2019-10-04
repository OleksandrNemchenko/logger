
#include <functional>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

#include <tests.h>
#include <avn/logger/logger_txt_file.h>

size_t test_logger_txt_file( void ){

    using namespace std;

    std::cout << "START test_txt_file" << std::endl;

    Logger::CLoggerTxtFile<wchar_t> log( L"/tmp/test.txt"s );
    const std::locale utf8_locale = locale(locale(), new codecvt_utf8<wchar_t>());

    log.Imbue( utf8_locale );
    log.AddLevelDescr( 0, L"TEST-0" );
    log.OnLevel( 0 );
    log.AddString( 0, L"This is test string : integer = ", 10 );


    return 0;
}