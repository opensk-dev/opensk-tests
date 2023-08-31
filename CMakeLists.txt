cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(opensk_tests LANGUAGES CXX)

if (NOT DEFINED sk_source_dir)
    message(FATAL_ERROR "cannot found opensk sources - sk_source_dir not set")
endif()

add_library(sk_tests_setup INTERFACE)
target_compile_features(sk_tests_setup INTERFACE cxx_std_23)
set(sk_tests_include_dir ${CMAKE_CURRENT_SOURCE_DIR}/common)
target_include_directories(
    sk_tests_setup
    INTERFACE
    "$<BUILD_INTERFACE:${sk_tests_include_dir}>"
    "$<BUILD_INTERFACE:${sk_source_dir}>"
)
target_sources(sk_tests_setup INTERFACE
    "${sk_tests_include_dir}/decorators.hpp"
)

# for boost-test
find_package(Boost 1.82 REQUIRED COMPONENTS unit_test_framework)
target_link_libraries(sk_tests_setup INTERFACE ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_include_directories(
    sk_tests_setup
    INTERFACE
    "$<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>"
)

enable_testing()

add_executable(
    sk_test_read_options
    "test_read_options.cpp"
    "${sk_source_dir}/options.hpp"
    "${sk_source_dir}/options.cpp"
)

target_link_libraries(sk_test_read_options sk_tests_setup)

add_executable(
    sk_test_runtime_arena
    "test_runtime_arena.cpp"
    "${sk_source_dir}/core/task.hpp"
    "${sk_source_dir}/core/task.cpp"
    "${sk_source_dir}/core/runtime_arena.hpp"
    "${sk_source_dir}/core/runtime_arena.cpp"
)

find_package(TBB REQUIRED)
target_link_libraries(sk_test_runtime_arena sk_tests_setup TBB::tbb)
