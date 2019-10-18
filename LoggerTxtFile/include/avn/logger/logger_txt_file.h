// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef _AVN_LOGGER_TXT_FILE_H_
#define _AVN_LOGGER_TXT_FILE_H_

#include <filesystem>
#include <fstream>

#include <avn/logger/logger_txt_base.h>

namespace Logger {

template<bool LockThr, typename TChar>
class CLoggerTxtFile : public CLoggerTxtBase<LockThr, TChar> {
public:
    using TBase = CLoggerTxtBase<LockThr, TChar>;
    using TString = std::basic_string<TChar>;
    using TStream = std::basic_ofstream<TChar>;

    CLoggerTxtFile( bool local_time = true ): CLoggerTxtBase<LockThr, TChar>(local_time)    {}
    CLoggerTxtFile( const std::filesystem::path &filename, std::ios_base::openmode mode = std::ios_base::out, bool local_time = true ) :
            CLoggerTxtFile( local_time )
    { OpenFile( filename, mode ); }

    CLoggerTxtFile& OpenFile( const std::filesystem::path &filename, std::ios_base::openmode mode = std::ios_base::out )    { _fstream.open( filename, mode ); return *this; }
    CLoggerTxtFile& CloseFile( const std::filesystem::path &filename, std::ios_base::openmode mode = std::ios_base::out )   { _fstream.close(); return *this; }
    CLoggerTxtFile& FlushFile()                               { _fstream.flush(); return *this; }
    CLoggerTxtFile& Imbue( const std::locale& loc )                 { _fstream.imbue( loc ); return *this; }
    TStream& Stream()                                         { return _fstream; }

    bool IsOpenedFile() const                                 { return _fstream.is_open(); }
    const TStream& Stream() const                             { return _fstream; }

private:

    bool OutStrings( std::size_t level, std::chrono::system_clock::time_point time, TString&& data ) override final;

    TStream _fstream;

};

template<bool LockThr, typename TChar>
bool CLoggerTxtFile<LockThr, TChar>::OutStrings( std::size_t level, std::chrono::system_clock::time_point time, TString &&data ){

    assert( _fstream.is_open() );

    if( _fstream.is_open() ){
        _fstream << TBase::PrepareString( level, time, std::move(data) ) << std::endl;

        return true;
    }

    return false;
}

} // namespace Logger

#endif  // _AVN_LOGGER_TXT_FILE_H_
