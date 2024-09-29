cmake_minimum_required(VERSION 3.14)
project(SimPHony)

set(USE_PYBIND11 ON)

cmake_host_system_information(RESULT _host_name QUERY HOSTNAME)

string(FIND ${_host_name} "bgc-jena.mpg" res)
if (res EQUAL 0)
    message("You appear to be on the cluster. Setting cluster env variables.")
    set(HOSTSPEC "cluster_mpi_bgc")
else()
    message("You appear to be on a local machine. Prepare to set the env variables yourself.")
    set(HOSTSPEC "local")
endif()

if ("${HOSTSPEC}" STREQUAL "cluster_mpi_bgc")
    set(PYTHON_EXECUTABLE /Net/Groups/BSI/work_scratch/ppapastefanou/envs/phs/bin/python)
else()
    # You prob have to adapt this path to match your environemnt
    set(PYTHON_EXECUTABLE /Users/pp/anaconda3/envs/PHS-post/bin/python)
endif()
#

find_package(pybind11 CONFIG REQUIRED)
message(STATUS "Found pybind11 v${pybind11_VERSION}: ${pybind11_INCLUDE_DIRS}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(APPLE)
    message("Using Mac")
elseif (UNIX)
    link_libraries(stdc++fs)
    message("Using Unix")
else()
    message("Using Win")
endif()

# ----------------------------------------------------
# Setting up SimPHony main model interface with python
# ----------------------------------------------------

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
add_subdirectory(src/core/cpp)
# Specify the executable to build, and which sources to build it from
pybind11_add_module(SimPHony src/core/cpp/pybind_interface.cpp
        ${sources})

# ----------------------------------------------------
# Setting up google tests
# ----------------------------------------------------

set(GTEST_VERSION 1.6.0 CACHE STRING "Google test version")
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY "https://github.com/google/googletest.git"
        GIT_TAG "main"
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(tests)

include(GoogleTest)
# add test exe target, and link it with gtest_main library
add_executable(SimPHony_tests ${sources} ${test_sources})
target_link_libraries(SimPHony_tests GTest::gtest_main)
gtest_discover_tests(SimPHony_tests)




##add_executable(hydro_standalone_test ${test_sources} ${sources})
#add_executable(hydro_standalone_test
#        tests/general/basic_test.h)
#target_link_libraries(
#        hydro_standalone_test
#        GTest::gtest_main
#)
#
#include(GoogleTest)
#gtest_discover_tests(hydro_standalone_test)
