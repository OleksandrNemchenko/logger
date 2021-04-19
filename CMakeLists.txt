
cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

project(Logger VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(LoggerBase)
add_subdirectory(LoggerTxtBase)
add_subdirectory(LoggerTxtFile)
add_subdirectory(LoggerTxtCOut)

add_subdirectory(Test)