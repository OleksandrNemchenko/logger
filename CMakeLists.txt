
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(Logger VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(LoggerBase)

add_subdirectory(LoggerTxt)

add_subdirectory(Test)