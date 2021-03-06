// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>

#include <tests.h>

int main(int argc, char *argv[])
{
    int ret_code = 0;

    std::cout << "Start ALogger library tests" << std::endl;

    ret_code += test_base();
    ret_code += test_txt_file();
    ret_code += test_txt_cout();
    ret_code += test_txt_group();

    return ret_code;
}