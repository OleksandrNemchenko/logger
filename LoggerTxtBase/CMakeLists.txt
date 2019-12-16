
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(avn_logger_txt VERSION 1.0.0 LANGUAGES CXX)

add_library(avn_logger_txt_base INTERFACE)

target_sources(avn_logger_txt_base
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/avn/logger/logger_txt_base.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/avn/logger/logger_txt_group.h
        )

target_link_libraries(avn_logger_txt_base
        INTERFACE
        avn_logger_base
        )

target_include_directories(avn_logger_txt_base
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )