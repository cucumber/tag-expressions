project(Cucumber-Tag-Expressions-Example)

add_executable(example src/basic_usage.cpp)
target_link_libraries(example PRIVATE cucumber::tag-expressions gcov)

