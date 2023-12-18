cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(opensk_tests LANGUAGES CXX)

if(NOT DEFINED sk_source_dir)
    message(FATAL_ERROR "cannot found opensk sources - sk_source_dir not set")
endif()

add_library(sk_tests_setup INTERFACE)
target_link_libraries(sk_tests_setup INTERFACE opensk_setup compile_flags_setup)

set(sk_tests_include_dir ${CMAKE_CURRENT_SOURCE_DIR}/common/include)
target_include_directories(sk_tests_setup INTERFACE "$<BUILD_INTERFACE:${sk_tests_include_dir}>")
target_sources(
    sk_tests_setup INTERFACE
    "${sk_tests_include_dir}/test/common.hpp"
    "${sk_tests_include_dir}/test/test_suite.hpp"
    "${sk_tests_include_dir}/test/decorators.hpp"
)

find_package(Boost 1.82 COMPONENTS unit_test_framework REQUIRED)

target_link_libraries(sk_tests_setup INTERFACE ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_include_directories(sk_tests_setup INTERFACE "$<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>")

add_executable(sk_test_read_options
    "test_read_options.cpp"
    "${sk_source_dir}/options.hpp"
    "${sk_source_dir}/options.cpp"
    "${sk_source_dir}/config.hpp"
    "${sk_source_dir}/config.cpp"
    "${sk_gen_include_dir}/config_def.hpp"
)
target_link_libraries(sk_test_read_options sk_tests_setup)
add_test(NAME test_read_options COMMAND sk_test_read_options)

add_executable(sk_test_frames
    "test_frames.cpp"
    "${sk_source_dir}/core/frame.hpp"
    "${sk_source_dir}/core/frame.cpp"
    "${sk_source_dir}/core/exceptions.hpp"
    "${sk_source_dir}/core/exceptions.cpp"
    "${sk_source_dir}/config.hpp"
    "${sk_source_dir}/config.cpp"
    "${sk_gen_include_dir}/config_def.hpp"
)
target_link_libraries(sk_test_frames sk_tests_setup)
add_test(NAME test_frames COMMAND sk_test_frames)

add_executable(sk_test_runtime_arena
    "test_runtime_arena.cpp"
    "${sk_source_dir}/core/task.hpp"
    "${sk_source_dir}/core/task.cpp"
    "${sk_source_dir}/core/runtime_arena.hpp"
    "${sk_source_dir}/core/runtime_arena.cpp"
)
target_link_libraries(sk_test_runtime_arena sk_tests_setup)
add_test(NAME test_runtime_arena COMMAND sk_test_runtime_arena)
