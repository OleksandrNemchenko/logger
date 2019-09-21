
#include <functional>
#include <iostream>
#include <string>
#include <thread>

#include <tests.h>
#include <avn/logger/logger_txt.h>

size_t test_logger_txt( void ){

    using namespace std;

    Logger::CLoggerTxt log( "/tmp/test.txt"s, false );

    log.AddLevelDescr( 0, "TEST-0" );
    log.OnLevel( 0 );
    log.AddToLog( 0, { "test"s, "string"s } );

    return 0;
}