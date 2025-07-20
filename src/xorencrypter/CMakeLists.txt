cmake_minimum_required(VERSION 3.10)
project(XorEncrypter VERSION 1.0)
set(lib_target ${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARU_DIR}/bin")
add_library(${lib_target} SHARED
    "src/IfEncrypter.h"
    "src/XorEncrypter.h"
    "src/XorEncrypter.cpp"
)

target_include_directories(${lib_target} PRIVATE "src")