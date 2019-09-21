
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(logger_txt VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(logger_txt)

target_sources(logger_txt
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/avn/logger/logger_txt.h

        ${CMAKE_CURRENT_SOURCE_DIR}/src/logger_txt.cpp
        )

target_link_libraries(logger_txt
        PRIVATE
        logger_base
        )

target_include_directories(logger_txt
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )