cmake_minimum_required(VERSION 3.16)
project(ls VERSION 1.0)
enable_testing()
set(CMAKE_CXX_STANDARD 17)
add_executable(ls main.cc ls.cc)
configure_file(config.h.in config.h)
target_include_directories(ls
    PUBLIC
        "${PROJECT_BINARY_DIR}"
)
target_precompile_headers(ls
    PUBLIC
        cxxopts.hpp
)
find_package(GTest REQUIRED)
include(GoogleTest)
add_executable(ls_test ls_test.cc)
target_link_libraries(ls_test GTest::GTest GTest::Main)
gtest_discover_tests(ls_test)
