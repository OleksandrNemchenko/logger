
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(Logger VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(LoggerBase)
add_subdirectory(LoggerTxtBase)
add_subdirectory(LoggerTxtFile)
add_subdirectory(LoggerTxtCout)

add_subdirectory(Test)