cmake_minimum_required(VERSION 3.4)
#set(CMAKE_C_COMPILER "gcc")
#set(CMAKE_CXX_COMPILER "g++")

set(USE_PYBIND11 ON)

#set(CMAKE_PREFIX_PATH "/Net/Groups/BSI/work_scratch/ppapastefanou/envs/forcing_gen/lib/python3.9/site-packages/pybind11")
#set(CMAKE_PREFIX_PATH "/Net/Groups/BSI/work_scratch/ppapastefanou/envs/forcing_gen/lib/python3.9/site-packages/mpi4py")

set(PYTHON_EXECUTABLE /Users/pp/anaconda3/envs/PHS-post/bin/python)

find_package(pybind11 CONFIG REQUIRED)
message(STATUS "Found pybind11 v${pybind11_VERSION}: ${pybind11_INCLUDE_DIRS}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(hydro_standalone)

if(APPLE)
    message("Using Mac")
elseif (UNIX)
    link_libraries(stdc++fs)
    message("Using Unix")
else()
    message("Using Win")
endif()



set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

add_subdirectory(src)

# Specify the executable to build, and which sources to build it from


pybind11_add_module(hydro_standalone src/pybind_interface.cpp ${sources} Tests/swiss_single_test.h Tests/swiss_single_test.cpp Tests/hainich_single_test.h Tests/hainich_single_test.cpp Tests/multi_test.h Tests/multi_test.cpp src/framework/parameters.h src/framework/parameters.cpp)

project(hydro_standalone_run)

add_executable(hydro_standalone_run ${sources} Tests/swiss_single_test.h Tests/swiss_single_test.cpp Tests/multi_test.h Tests/multi_test.cpp Tests/hainich_single_test.h Tests/hainich_single_test.cpp)

