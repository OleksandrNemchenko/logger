
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(logger_txt VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(logger_txt_base)

target_sources(logger_txt_base
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/avn/logger/logger_txt_base.h

        ${CMAKE_CURRENT_SOURCE_DIR}/src/logger_txt_base.cpp
        )

target_link_libraries(logger_txt_base
        PRIVATE
        logger_base
        )

target_include_directories(logger_txt_base
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )