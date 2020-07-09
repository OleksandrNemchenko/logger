
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(test_logger VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(test_logger)

target_compile_features(test_logger
        PRIVATE
        )

target_sources(test_logger
        PRIVATE
        main.cpp
        src/logger_base.cpp
        src/logger_txt_base.cpp
        src/logger_txt_file.cpp
        src/logger_txt_cout.cpp
        src/logger_txt_group.cpp
        )

target_include_directories(test_logger
        PRIVATE
        include
        )

target_link_libraries(test_logger
        PRIVATE
        avn_logger_base
        avn_logger_txt_base
        avn_logger_txt_file
        avn_logger_txt_cout
        )
