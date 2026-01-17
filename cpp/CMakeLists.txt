cmake_minimum_required(VERSION 3.15)
project(Cucumber-Tag-Expressions VERSION 8.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Library target
add_library(cucumber-tag-expressions)
add_library(cucumber::tag-expressions ALIAS cucumber-tag-expressions)
target_sources(cucumber-tag-expressions
    PRIVATE
        src/expression.cpp
        src/parser.cpp
)

target_include_directories(cucumber-tag-expressions
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Example executable
add_executable(example examples/basic_usage.cpp)
target_link_libraries(example PRIVATE cucumber::tag-expressions)

# Main project check
set(CUCUMBER_TAG_EXPRESSIONS_MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CUCUMBER_TAG_EXPRESSIONS_MAIN_PROJECT ON)
endif()

# Options
if(${CUCUMBER_TAG_EXPRESSIONS_MAIN_PROJECT})
    set(CUCUMBER_TAG_EXPRESSIONS_BUILD_TESTS_INIT ON)
    set(CUCUMBER_TAG_EXPRESSIONS_ENABLE_COVERAGE_INIT ON)
else()
    set(CUCUMBER_TAG_EXPRESSIONS_BUILD_TESTS_INIT OFF)
    set(CUCUMBER_TAG_EXPRESSIONS_ENABLE_COVERAGE_INIT OFF)
endif()

option(
    CUCUMBER_TAG_EXPRESSIONS_BUILD_TESTS
    "Enable build of the tests"
    ${CUCUMBER_TAG_EXPRESSIONS_BUILD_TESTS_INIT}
)

option(
    CUCUMBER_TAG_EXPRESSIONS_ENABLE_COVERAGE
    "Enable compiler flags for code coverage measurements"
    ${CUCUMBER_TAG_EXPRESSIONS_ENABLE_COVERAGE_INIT}
)

if(CUCUMBER_TAG_EXPRESSIONS_BUILD_TESTS)
    if(CUCUMBER_TAG_EXPRESSIONS_ENABLE_COVERAGE)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(cucumber-tag-expressions PRIVATE --coverage -O0 -g)
            target_link_libraries(cucumber-tag-expressions PRIVATE --coverage)
        endif()
    endif()

    enable_testing()

    # Find Google Test (if available)
    find_package(GTest QUIET)
    if(GTest_FOUND)
        add_executable(tag_expressions_test
            tests/test_errors.cpp
            tests/test_evaluations.cpp
            tests/test_parsing.cpp
            tests/test_precedence.cpp
        )
        target_link_libraries(tag_expressions_test
            PRIVATE
                cucumber::tag-expressions
                GTest::gtest
                GTest::gtest_main
        )
        
        add_test(NAME tag_expressions_test COMMAND tag_expressions_test)
    endif()
endif()

# Installation rules
install(TARGETS cucumber-tag-expressions
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
