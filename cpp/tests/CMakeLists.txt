project(Cucumber-Tag-Expressions-Tests)

if(CUCUMBER_TAG_EXPRESSIONS_BUILD_TESTS)
    enable_testing()

    # Find Google Test (if available)
    find_package(GTest QUIET)
    if(GTest_FOUND)
        add_executable(tag_expressions_test
            src/test_errors.cpp
            src/test_evaluations.cpp
            src/test_expression.cpp
            src/test_parser.cpp
            src/test_parsing.cpp
            src/test_precedence.cpp
        )
        
        # Apply coverage flags to test executable
        if(CUCUMBER_TAG_EXPRESSIONS_ENABLE_COVERAGE)
            if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
                target_compile_options(tag_expressions_test PRIVATE -fprofile-arcs -ftest-coverage --coverage -O0 -g)
                target_link_options(tag_expressions_test PRIVATE -fprofile-arcs -ftest-coverage --coverage)
            endif()
        endif()
        
        target_link_libraries(tag_expressions_test
            PRIVATE
                cucumber::tag-expressions
                GTest::gtest
                GTest::gtest_main
                gcov
        )
        
        add_test(NAME tag_expressions_test COMMAND tag_expressions_test)
    endif()
endif()

