
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(avn_logger_base VERSION 0.2.0 LANGUAGES CXX)

add_library(avn_logger_base INTERFACE)

target_sources(avn_logger_base
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/avn/logger/base_thr_safety.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/avn/logger/data_types.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/avn/logger/logger_base.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/avn/logger/logger_group.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/avn/logger/logger_group_task.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/avn/logger/logger_task.h
        )

target_include_directories(avn_logger_base
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )

target_link_libraries(avn_logger_base
        INTERFACE
#        pthread
        )

include(GNUInstallDirs)

install(TARGETS avn_logger_base
        EXPORT avn_logger_base
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )

#install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/
#        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
#        )