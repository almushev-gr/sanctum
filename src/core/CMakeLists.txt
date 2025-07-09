cmake_minimum_required(VERSION 3.10)
project(SanctumDefaultCore VERSION 1.0)
set(lib_target ${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARU_DIR}/bin")
add_library(${lib_target}
    "src/IfSanctumCore.h"
    "src/IfEncrypter.h"
    "src/SanctumDefaultCore.h"
    "src/SanctumDefaultCore.cpp"
    "src/FileInsideSanctum.h"
    "src/FileInsideSanctum.cpp"
    "src/DefaultEncrypter.h"
    "src/DefaultEncrypter.cpp"
    "src/ContentsTable.h"
    "src/ContentsTable.cpp"
)

target_include_directories(${lib_target} PRIVATE "src")