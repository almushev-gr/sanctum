cmake_minimum_required(VERSION 3.10)
project(sanctum VERSION 1.0)
set(exe_target ${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARU_DIR}/bin")
add_subdirectory("src/core")
add_subdirectory("src/xorencrypter")
add_executable(${exe_target}
    "src/main.cpp"
    "src/cmds/IfCommand.h"
    "src/cmds/CommandNomenclature.h"
    "src/cmds/CommandNomenclature.cpp"
    "src/cmds/SanctumDirCommand.h"
    "src/cmds/SanctumDirCommand.cpp"
    "src/cmds/WorkDirCommand.h"
    "src/cmds/WorkDirCommand.cpp"
    "src/cmds/PutCommand.h"
    "src/cmds/PutCommand.cpp"
    "src/cmds/SaveConfigCommand.h"
    "src/cmds/SaveConfigCommand.cpp"
    "src/cmds/Command.h"
    "src/cmds/Command.cpp"
    "src/cmds/ContentCommand.h"
    "src/cmds/ContentCommand.cpp"
    "src/cmds/CommitCommand.h"
    "src/cmds/CommitCommand.cpp"
    "src/cmds/EncryptorCommand.h"
    "src/cmds/EncryptorCommand.cpp"
    "src/cmds/GetCommand.h"
    "src/cmds/GetCommand.cpp"
    "src/cmds/KeyCommand.h"
    "src/cmds/KeyCommand.cpp"
    "src/cmds/SanctumNameCommand.h"
    "src/cmds/SanctumNameCommand.cpp"
    "src/core/src/IfSanctumCore.h"
    "src/cmds/ConsoleTable.h"
    "src/cmds/ConsoleTable.cpp"
)
target_link_libraries(sanctum SanctumDefaultCore)
target_include_directories(${exe_target} PRIVATE "src" "src/core/src")