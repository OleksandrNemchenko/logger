
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(logger_txt VERSION 1.0.0 LANGUAGES CXX)

add_library(logger_txt_base INTERFACE)

target_sources(logger_txt_base
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/avn/logger/logger_txt_base.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/avn/logger/logger_txt_group.h
        )

target_link_libraries(logger_txt_base
        INTERFACE
        logger_base
        )

target_include_directories(logger_txt_base
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )