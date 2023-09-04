cmake_minimum_required(VERSION 3.4)
#set(CMAKE_C_COMPILER "gcc")
#set(CMAKE_CXX_COMPILER "g++")

set(USE_PYBIND11 ON)

#set(PYTHON_EXECUTABLE /Users/pp/opt/anaconda3/bin/python3.9)

find_package(pybind11 CONFIG REQUIRED)
message(STATUS "Found pybind11 v${pybind11_VERSION}: ${pybind11_INCLUDE_DIRS}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(hydro_standalone)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

add_subdirectory(src/cpp)

# Specify the executable to build, and which sources to build it from


pybind11_add_module(hydro_standalone src/cpp/pybind_interface.cpp ${sources} src/Tests/single_test.h src/Tests/single_test.cpp src/Tests/multi_test.h src/Tests/multi_test.cpp)

project(hydro_standalone_run)

add_executable(hydro_standalone_run ${sources} src/Tests/single_test.h src/Tests/single_test.cpp src/Tests/multi_test.h src/Tests/multi_test.cpp)

