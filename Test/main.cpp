
#include <iostream>

#include <tests.h>

int main(int argc, char *argv[]){
    int ret_code = 0;

    std::cout << "Start Logger library tests" << std::endl;

    ret_code += test_logger_base();
    ret_code += test_logger_txt_file();
    ret_code += test_logger_txt_cout();

    return ret_code;
}