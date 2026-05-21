file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(serializen_test ${SOURCES})

set_target_properties(serializen_test PROPERTIES
    CXX_STANDARD 26
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_compile_options(serializen_test PRIVATE
        "-freflection"
        "-std=c++26"
)

target_link_libraries(serializen_test PRIVATE serializen)

add_test(
        NAME test
        COMMAND serializen_test
)