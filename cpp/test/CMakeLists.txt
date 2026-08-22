add_executable(cucumber_tag_expressions.test)

target_link_libraries(cucumber_tag_expressions.test PUBLIC
    cucumber_tag_expressions_lib
    GTest::gmock
    GTest::gmock_main
    yaml-cpp::yaml-cpp
)

target_sources(cucumber_tag_expressions.test PRIVATE
    TestErrors.cpp
    TestEvaluations.cpp
    TestParsing.cpp
    TestToken.cpp
)

target_compile_definitions(cucumber_tag_expressions.test
    PRIVATE
        TESTDATA_SRC="${PROJECT_SOURCE_DIR}/../testdata"
)

include(GoogleTest)
gtest_discover_tests(cucumber_tag_expressions.test)
