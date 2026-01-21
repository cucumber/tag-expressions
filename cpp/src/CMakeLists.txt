project(Cucumber-Tag-Expressions-Lib)

add_library(cucumber-tag-expressions)
add_library(cucumber::tag-expressions ALIAS cucumber-tag-expressions)

target_sources(cucumber-tag-expressions
    PRIVATE
        lib/expression.cpp
        lib/parser.cpp
)

# Apply coverage flags to library
if(CUCUMBER_TAG_EXPRESSIONS_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(cucumber-tag-expressions PRIVATE -fprofile-arcs -ftest-coverage --coverage -O0 -g)
        target_link_options(cucumber-tag-expressions PRIVATE -fprofile-arcs -ftest-coverage --coverage)
    endif()

    target_link_libraries(cucumber-tag-expressions
        PRIVATE
            gcov
    )
endif()

target_include_directories(cucumber-tag-expressions
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<INSTALL_INTERFACE:include>
)

