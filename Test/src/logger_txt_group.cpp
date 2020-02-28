// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <codecvt>
#include <cstdio>
#include <functional>
#include <iostream>
#include <string>
#include <filesystem>
#include <tests.h>
#include <avn/logger/logger_txt_file.h>
#include <avn/logger/logger_txt_cout.h>
#include <avn/logger/logger_txt_group.h>

size_t test_txt_group()
{
    using namespace std;
    namespace fs = std::filesystem;

    fs::path tmpFile;
    size_t ctr = 0;

    do {
        tmpFile = fs::temp_directory_path() / ( std::to_wstring(ctr) + L".tmp"s );
        if (!fs::exists(tmpFile))
            break;
        ++ctr;
    }
    while(true);

    std::wcout << L"START test_txt_group with file "s << tmpFile << std::endl;

#ifdef TEST_ERROR_2
    class ALoggerTest_ERROR1 : public ALogger::ALoggerBase<true, std::wstring>{ bool outData(std::size_t level, std::chrono::system_clock::time_point time, const std::wstring& data) noexcept override { return true; } };
#endif
    ALogger::ALoggerTxtGroup<ALogger::ALoggerTxtFile<true, wchar_t>, ALogger::ALoggerTxtCOut<false, wchar_t>
#ifdef TEST_ERROR_2
                    , ALoggerTest_ERROR1
#endif
                    > log;

    const std::locale utf8_locale = locale(locale(), new codecvt_utf8<wchar_t>());

    log.imbue(utf8_locale);
    log.logger<0>().openFile(tmpFile);
    log.addLevelDescr(0, L"TEST-0");
    log.enableLevel(0);
    log.addString(0, L"This is test string : integer = ", 10);

//    std::filesystem::remove(tmpFile);

    return 0;
}