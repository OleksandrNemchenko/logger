
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(avn_logger_txt_cout VERSION 1.0.0 LANGUAGES CXX)

add_library(avn_logger_txt_cout INTERFACE)

target_sources(avn_logger_txt_cout
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/avn/logger/logger_txt_cout.h
        )

target_link_libraries(avn_logger_txt_cout
        INTERFACE
        avn_logger_txt_base
        )

target_include_directories(avn_logger_txt_cout
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )