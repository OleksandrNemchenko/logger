
#ifndef _AVN_LOGGER_TXT_FILE_H_
#define _AVN_LOGGER_TXT_FILE_H_

#include <filesystem>
#include <fstream>

#include <avn/logger/logger_txt_base.h>

namespace Logger {

template<typename TChar>
class CLoggerTxtFile : public CLoggerTxtBase<TChar> {
public:
    using TBase = CLoggerTxtBase<TChar>;
    using TString = std::basic_string<TChar>;

    CLoggerTxtFile( bool local_time = true ): CLoggerTxtBase<TChar>(local_time)    {}
    CLoggerTxtFile( const std::filesystem::path &filename, std::ios_base::openmode mode = std::ios_base::out, bool local_time = true ) :
            CLoggerTxtFile( local_time )
    { OpenFile( filename, mode ); }

    CLoggerTxtFile& OpenFile( const std::filesystem::path &filename, std::ios_base::openmode mode = std::ios_base::out )    { _fstream.open( filename, mode ); return *this; }
    bool IsOpenedFile( void ) const                                 { return _fstream.is_open(); }
    CLoggerTxtFile& CloseFile( const std::filesystem::path &filename, std::ios_base::openmode mode = std::ios_base::out )    { _fstream.close(); return *this; }
    CLoggerTxtFile& FlushFile( void )                               { _fstream.flush(); return *this; }
    CLoggerTxtFile& Imbue( const std::locale& loc )                 { _fstream.imbue( loc ); return *this; }

private:

    bool OutStrings( std::size_t level, std::chrono::system_clock::time_point time, TString &&data ) override final;

    std::basic_ofstream<TChar> _fstream;

};

template<typename TChar>
bool CLoggerTxtFile<TChar>::OutStrings( std::size_t level, std::chrono::system_clock::time_point time, TString &&data ){

    assert( _fstream.is_open() );

    if( _fstream.is_open() ){
        _fstream << TBase::PrepareString( level, time, std::move(data) ) << std::endl;

        return true;
    }

    return false;
}

} // namespace Logger

#endif  // _AVN_LOGGER_TXT_FILE_H_
