// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <functional>
#include <iostream>

#include <tests.h>
#include <avn/logger/logger_txt_cout.h>

size_t test_txt_cout(){

    using namespace std;

    std::cout << "START test_txt_cout" << std::endl;

    Logger::CLoggerTxtCOut<false, wchar_t> wlog;
    Logger::CLoggerTxtCOut<true, char> log;

    wlog.AddLevelDescr( 0, L"TEST-0" );
    wlog.OnLevel( 0 );
    wlog.AddString( 0, L"This is test wstring : integer = ", 10 );

    log.AddLevelDescr( 0, "TEST-0" );
    log.OnLevel( 0 );
    log.AddString( 0, "This is test string : integer = ", 10 );


    return 0;
}