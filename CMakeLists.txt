cmake_minimum_required(VERSION 3.5)

project(set)

set(CMAKE_CXX_STANDARD 17)

add_library(gtest
        gtest/gtest.h
        gtest/gtest-all.cc
        gtest/gtest_main.cc)

add_library(counted counted.h counted.cpp fault_injection.h fault_injection.cpp)
target_link_libraries(counted gtest)


add_executable(set_testing main.cpp set.hpp fault_injection.h fault_injection.cpp)
target_link_libraries(set_testing gtest counted -lpthread)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
endif()
