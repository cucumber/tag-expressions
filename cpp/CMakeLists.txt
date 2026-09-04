# The VERSION here is a placeholder; the real version is in the VERSION file.
set(VERSION 1.0.0)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")
    file(STRINGS "VERSION" LINES)
    list(GET LINES 0 VERSION)
endif()

# Crude Semver parsing
if("${VERSION}" MATCHES "^([^\\.]+)\\.([^\\.]+)\\.([^\\.]+)$")
    set(VER_MAJOR ${CMAKE_MATCH_1})
    set(VER_MINOR ${CMAKE_MATCH_2})
    set(VER_PATCH ${CMAKE_MATCH_3})
else()
    message(FATAL_ERROR "unable to parse version: ${VERSION}")
endif()

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(cucumber_tag_expressions VERSION ${VERSION} LANGUAGES CXX)

# Options
if (POLICY CMP0077)
    # Allow CMake 3.13+ to override options when using FetchContent / add_subdirectory.
    cmake_policy(SET CMP0077 NEW)
endif ()

option(CUCUMBER_TAG_EXPRESSIONS_BUILD_TESTS "Build the unit tests." Off)
option(CUCUMBER_TAG_EXPRESSIONS_ENABLE_CLANG_TIDY "Run clang-tidy as part of the build." Off)
option(CUCUMBER_TAG_EXPRESSIONS_ENABLE_COVERAGE "Enable compiler flags for code coverage measurements." Off)
option(CUCUMBER_TAG_EXPRESSIONS_ENABLE_SANITIZERS "Enable compiler flags for the address and undefined behaviour sanitizers." Off)

if(CUCUMBER_TAG_EXPRESSIONS_ENABLE_SANITIZERS AND CUCUMBER_TAG_EXPRESSIONS_ENABLE_COVERAGE)
    message(FATAL_ERROR "CUCUMBER_TAG_EXPRESSIONS_ENABLE_SANITIZERS and CUCUMBER_TAG_EXPRESSIONS_ENABLE_COVERAGE are mutually exclusive.")
endif()

if(CUCUMBER_TAG_EXPRESSIONS_ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_PROGRAM clang-tidy REQUIRED)
endif()

if(CUCUMBER_TAG_EXPRESSIONS_ENABLE_COVERAGE)
    find_program(GCOVR_PROGRAM gcovr REQUIRED)

    add_custom_target(
        generate-lcov-report
        COMMAND ${GCOVR_PROGRAM} --version
        COMMAND ${GCOVR_PROGRAM} --root ${CMAKE_CURRENT_SOURCE_DIR} --merge-lines --delete --exclude-unreachable-branches --exclude-throw-branches --exclude-noncode-lines --filter include/ --filter src/
            --txt-summary
            --lcov ${CMAKE_CURRENT_BINARY_DIR}/lcov.info
            --cobertura ${CMAKE_CURRENT_BINARY_DIR}/cobertura.xml --cobertura-pretty
            --markdown-summary ${CMAKE_CURRENT_BINARY_DIR}/coverage-summary.md
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating code coverage reports"
    )
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler cache
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
else()
    find_program(SCCACHE_PROGRAM sccache)
    if(SCCACHE_PROGRAM)
        set(CMAKE_CXX_COMPILER_LAUNCHER "${SCCACHE_PROGRAM}")
        message(STATUS "Using sccache: ${SCCACHE_PROGRAM}")
    endif()
endif()

# Configuration
include(GNUInstallDirs)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS)
endif()

# Target options
if(CUCUMBER_TAG_EXPRESSIONS_ENABLE_CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_PROGRAM}")
endif()

if(CUCUMBER_TAG_EXPRESSIONS_ENABLE_COVERAGE)
    add_compile_options(
        -g -O0 --coverage -fprofile-arcs -ftest-coverage -fno-inline
        $<$<COMPILE_LANGUAGE:CXX>:-fno-elide-constructors>
    )

    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        link_libraries(gcov)
    else()
        add_link_options(--coverage)
    endif()
endif()

if(CUCUMBER_TAG_EXPRESSIONS_ENABLE_SANITIZERS)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-fsanitize=address -fsanitize=undefined)
        add_link_options(-fsanitize=address -fsanitize=undefined)
    endif()
endif()

# Targets
add_library(cucumber_tag_expressions_lib)
add_library(cucumber::tag_expressions ALIAS cucumber_tag_expressions_lib)

target_sources(
    cucumber_tag_expressions_lib
    PRIVATE
        include/cucumber/tag-expressions/Error.hpp
        include/cucumber/tag-expressions/Model.hpp
        include/cucumber/tag-expressions/Parser.hpp
        include/cucumber/tag-expressions/Token.hpp

        src/cucumber/tag-expressions/Error.cpp
        src/cucumber/tag-expressions/Model.cpp
        src/cucumber/tag-expressions/Parser.cpp
        src/cucumber/tag-expressions/Token.cpp
)

target_include_directories(
    cucumber_tag_expressions_lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/
)

set_target_properties(
    cucumber_tag_expressions_lib
    PROPERTIES
        VERSION ${VERSION}
        SOVERSION ${VER_MAJOR}
        EXPORT_NAME tag_expressions
        OUTPUT_NAME cucumber_tag_expressions
)

# Installation support
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "cmake/cucumber_tag_expressions-config.cmake.in"
    "${PROJECT_BINARY_DIR}/cucumber_tag_expressions-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cucumber_tag_expressions"
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
)

write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/cucumber_tag_expressions-config-version.cmake"
    COMPATIBILITY AnyNewerVersion
)

install(
    TARGETS cucumber_tag_expressions_lib
    EXPORT cucumber_tag_expressions-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
    EXPORT cucumber_tag_expressions-targets
    FILE cucumber_tag_expressions-targets.cmake
    NAMESPACE cucumber::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cucumber_tag_expressions"
)

install(
    FILES
        "${PROJECT_BINARY_DIR}/cucumber_tag_expressions-config.cmake"
        "${PROJECT_BINARY_DIR}/cucumber_tag_expressions-config-version.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cucumber_tag_expressions"
)

install(
    DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Optional: tests
if(CUCUMBER_TAG_EXPRESSIONS_BUILD_TESTS)
    find_package(GTest REQUIRED)
    find_package(yaml-cpp REQUIRED)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()
