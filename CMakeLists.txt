cmake_minimum_required(VERSION 3.5)

project(set)

set(CMAKE_CXX_STANDARD 17)

add_library(gtest
        gtest/gtest.h
        gtest/gtest-all.cc gtest/gtest_main.cc)

add_executable(set_testing set_testing.cpp set.hpp)
target_link_libraries(set_testing gtest -lpthread)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
endif()
