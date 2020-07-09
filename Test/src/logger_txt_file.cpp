// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*
#include <codecvt>
#include <cstdio>
#include <functional>
#include <iostream>
#include <string>
#include <filesystem>
#include <tests.h>
#include <avn/logger/logger_txt_file.h>

size_t test_txt_file()
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

    std::wcout << L"START test_txt_file "s << tmpFile << std::endl;

    ALogger::ALoggerTxtFile<true, wchar_t> log(tmpFile);
    const std::locale utf8_locale = locale(locale(), new codecvt_utf8<wchar_t>());

    log.imbue(utf8_locale);
    log.addLevelDescr(0, L"TEST-0");
    log.enableLevel(0);
    log.addString(0, L"This is test string : integer = ", 10);

//    std::filesystem::remove(tmpFile);

    return 0;
}
*/