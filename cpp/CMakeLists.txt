cmake_minimum_required(VERSION 3.15)
project(cucumber-tag-expressions VERSION 8.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compiler warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Library target
add_library(cucumber_tag_expressions
    src/expression.cpp
    src/parser.cpp
)

target_include_directories(cucumber_tag_expressions
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Example executable
add_executable(example examples/basic_usage.cpp)
target_link_libraries(example PRIVATE cucumber_tag_expressions)

# Enable testing
enable_testing()

# Find Google Test (if available)
find_package(GTest QUIET)
if(GTest_FOUND)
    add_executable(tag_expressions_test
        tests/test_expression.cpp
        tests/test_parser.cpp
    )
    target_link_libraries(tag_expressions_test
        PRIVATE
            cucumber_tag_expressions
            GTest::gtest
            GTest::gtest_main
    )
    
    add_test(NAME tag_expressions_test COMMAND tag_expressions_test)
endif()

# Installation rules
install(TARGETS cucumber_tag_expressions
    EXPORT cucumber-tag-expressions-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/
    DESTINATION include
)

install(EXPORT cucumber-tag-expressions-targets
    FILE cucumber-tag-expressions-targets.cmake
    NAMESPACE cucumber::
    DESTINATION lib/cmake/cucumber-tag-expressions
)

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cucumber-tag-expressions-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cucumber-tag-expressions-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cucumber-tag-expressions-config.cmake"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cucumber-tag-expressions-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cucumber-tag-expressions-config-version.cmake"
    DESTINATION lib/cmake/cucumber-tag-expressions
)
