cmake_minimum_required(VERSION 3.20)
project(Project_Pointer)

execute_process (
        COMMAND bash -c "llvm-config --cxxflags'"
        OUTPUT_VARIABLE outVar
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${outVar} -g -O0 -fPIC -shared -rdynamic -dylib" )

add_library(Project_Pointer SHARED main.cpp)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})


# Link against LLVM libraries
target_link_libraries(Project_Pointer ${llvm_libs})
