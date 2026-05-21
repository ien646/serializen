cmake_minimum_required(VERSION 4.0)

project(serializen LANGUAGES CXX)

add_library(serializen INTERFACE)

target_include_directories(serializen INTERFACE include)

set_target_properties(serializen PROPERTIES
        CXX_STANDARD 26
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

target_compile_options(serializen INTERFACE
        "-freflection"
        "-std=c++26" # -- CMake pls...
)

option(SERIALIZEN_BUILD_TESTS "Build tests" ON)

enable_testing()
add_subdirectory(test)