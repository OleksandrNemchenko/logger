// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef _AVN_LOGGER_TESTS_H_
#define _AVN_LOGGER_TESTS_H_

#include <functional>

namespace ALogger {
    size_t tests();

    size_t test_base();
    size_t test_txt_base();
//    size_t test_txt_file();
//    size_t test_txt_cout();
//    size_t test_txt_group();

    size_t UnitTesting(std::string_view uutTitle, std::string_view errorDescription, std::function<bool()> testAction);

}

#endif  // _AVN_LOGGER_TESTS_H_
