
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(test_logger VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(test_logger)

target_compile_features(test_logger
        PRIVATE
        cxx_std_17)

target_sources(test_logger
        PRIVATE
        main.cpp
        src/logger_base.cpp
        )

target_include_directories(test_logger
        PRIVATE
        include
        )

target_link_libraries(test_logger
        PUBLIC
        logger_base
        )