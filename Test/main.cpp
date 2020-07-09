// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>

#include <tests.h>

namespace ALogger{

    size_t UnitTesting(std::string_view uutTitle, std::string_view errorDescription, std::function<bool()> testAction)
    {
        using namespace std::string_literals;

        if (testAction())
            return 0;

        std::cout << "[ERROR] "s << uutTitle << " : "s << errorDescription << std::endl;

        return 1;
    }

    size_t tests()
    {
        size_t ret_code = 0;
   
        ret_code += test_base();
        ret_code += test_txt_base();
//        ret_code += test_txt_file();
//        ret_code += test_txt_cout();
//        ret_code += test_txt_group();
    
        return ret_code;
    }
}