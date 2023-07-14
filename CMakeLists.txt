cmake_minimum_required(VERSION 3.4)
#set(CMAKE_C_COMPILER "gcc")
#set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(hydro_standalone)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

add_subdirectory(src/cpp)


set(USE_OPTION ON)

# Specify the executable to build, and which sources to build it from
add_executable(hydro_model ${sources})
