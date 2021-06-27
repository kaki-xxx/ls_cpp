cmake_minimum_required(VERSION 3.16)
project(ls VERSION 0.1)
enable_testing()
set(CMAKE_CXX_STANDARD 17)
add_executable(ls main.cc ls.cc)
target_precompile_headers(ls
    PUBLIC
        cxxopts.hpp
)
find_package(GTest REQUIRED)
include(GoogleTest)
add_executable(ls_test ls_test.cc)
target_link_libraries(ls_test GTest::GTest GTest::Main)
gtest_discover_tests(ls_test)
