cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(opensk_tests LANGUAGES CXX)

if(NOT DEFINED sk_source_dir)
    message(FATAL_ERROR "cannot found opensk sources - sk_source_dir not set")
endif()

add_library(sk_tests_setup INTERFACE)
target_compile_features(sk_tests_setup INTERFACE cxx_std_23)

target_link_libraries(sk_tests_setup INTERFACE opensk_lib)

set(sk_tests_include_dir ${CMAKE_CURRENT_SOURCE_DIR}/common/include)
target_include_directories(sk_tests_setup INTERFACE "$<BUILD_INTERFACE:${sk_tests_include_dir}>")
target_sources(
    sk_tests_setup INTERFACE
    "${sk_tests_include_dir}/test/common.hpp"
    "${sk_tests_include_dir}/test/test_suite.hpp"
    "${sk_tests_include_dir}/test/decorators.hpp"
)

# for boost-test
find_package(Boost 1.82 REQUIRED COMPONENTS unit_test_framework)
target_link_libraries(sk_tests_setup INTERFACE ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_include_directories(sk_tests_setup INTERFACE "$<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>")

add_executable(sk_test_read_options "test_read_options.cpp")
target_link_libraries(sk_test_read_options sk_tests_setup)
add_test(NAME test_read_options COMMAND sk_test_read_options)

add_executable(sk_test_frames "test_frames.cpp")
target_link_libraries(sk_test_frames sk_tests_setup)
add_test(NAME test_frames COMMAND sk_test_frames)

add_executable(sk_test_runtime_arena "test_runtime_arena.cpp")
target_link_libraries(sk_test_runtime_arena sk_tests_setup)
add_test(NAME test_runtime_arena COMMAND sk_test_runtime_arena)
