
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(logger_base VERSION 1.0.0 LANGUAGES CXX)

add_library(logger_base INTERFACE)

target_sources(logger_base
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/avn/logger/base_thr_safety.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/avn/logger/data_types.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/avn/logger/logger_base.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/avn/logger/logger_group.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/avn/logger/task.h
        )

target_include_directories(logger_base
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )