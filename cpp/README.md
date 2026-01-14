# Cucumber Tag Expressions - C++

<p align="center">
  <b>Readable boolean expressions to select features and scenarios marked with tags in Gherkin</b>
</p>

```gherkin
# -- SIMPLE TAG-EXPRESSION EXAMPLES:
@a and @b
@a or @b
not @a

# -- MORE TAG-EXPRESSION EXAMPLES:
@a and not @b
(@a or @b) and not @c
```

## Overview

This is a C++17 implementation of Cucumber Tag Expressions, providing a tag-expression parser and evaluation logic for cucumber/behave and other BDD frameworks.

Tag expressions are used to select features, scenarios, etc. in Gherkin files based on their tags. These selected items are normally included in a test run.

## Features

- **Boolean Operations**: `and`, `or`, `not`
- **Parentheses Support**: Group expressions with `(` and `)`
- **Escaped Characters**: Support for tags with spaces and special characters
- **STL-based**: Uses standard C++ containers (`std::set`, `std::string`, `std::shared_ptr`)
- **Exception Safety**: Provides detailed error messages for invalid expressions
- **Header-only Option**: Can be used as a library or included directly

## Building

### Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.15 or later
- (Optional) Google Test for running tests

### Build Instructions

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Run example
./build/example

# Run tests (if Google Test is available)
./build/tag_expressions_test
```

### Installation

```bash
cmake --build build --target install
```

## Usage

### Basic Example

```cpp
#include <cucumber_tag_expressions/parser.hpp>
#include <iostream>
#include <set>

using namespace cucumber_tag_expressions;

int main() {
    // Parse a tag expression
    auto expression = parse("@fast and not @slow");
    
    // Evaluate against a set of tags
    std::set<std::string> tags = {"@fast", "@unit"};
    bool matches = expression->evaluate(tags);
    
    std::cout << "Matches: " << (matches ? "yes" : "no") << "\n";
    // Output: Matches: yes
    
    return 0;
}
```

### More Examples

#### Tagged with @fast

```cpp
auto fast = parse("@fast");
fast->evaluate({"@fast", "@wip"});           // true
fast->evaluate({"@Fast"});                   // false
fast->evaluate({"@performance", "@slow"});   // false
```

#### Tagged with @wip and not @slow

```cpp
auto wip_not_slow = parse("@wip and not @slow");
wip_not_slow->evaluate({"@wip", "@home"});              // true
wip_not_slow->evaluate({"@wip", "@slow"});              // false
wip_not_slow->evaluate({"wet", "warm", "raining"});     // false
```

#### Tagged with both @fast and @integration

```cpp
auto fast_integration = parse("@integration and @fast");
fast_integration->evaluate({"@integration", "@fast", "@other"});  // true
fast_integration->evaluate({"@system", "@fast"});                 // false
```

#### Tagged with either @login or @registration

```cpp
auto auth_pages = parse("@login or @registration");
auth_pages->evaluate({"@account", "@login"});      // true
auth_pages->evaluate({"@admin", "@account"});      // false
```

#### Complex expressions with parentheses

```cpp
auto complex = parse("(@fast or @slow) and not @broken");
complex->evaluate({"@fast", "@passing"});  // true
complex->evaluate({"@slow", "@broken"});   // false
```

### Error Handling

The parser throws `TagExpressionError` for invalid expressions:

```cpp
try {
    auto expr = parse("@fast and and @slow");  // Invalid: double operator
} catch (const TagExpressionError& e) {
    std::cout << "Parse error: " << e.what() << "\n";
}
```

### Expression String Representation

You can convert an expression back to a string:

```cpp
auto expr = parse("@a and (@b or @c)");
std::string str = expr->to_string();
// str: "( a and ( b or c ) )"
```

### Using as a Library in Your Project

#### CMake Integration

```cmake
find_package(cucumber-tag-expressions REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE cucumber::cucumber_tag_expressions)
```

#### Manual Integration

If you prefer not to install the library, you can add it as a subdirectory:

```cmake
add_subdirectory(path/to/cucumber-tag-expressions)
target_link_libraries(my_app PRIVATE cucumber_tag_expressions)
```

## API Reference

### Main Functions

#### `parse(const std::string& text)`

Parses a tag expression string and returns an expression tree.

- **Parameters**: `text` - Tag expression as a string
- **Returns**: `std::shared_ptr<Expression>` - Parsed expression tree
- **Throws**: `TagExpressionError` - If the expression is invalid

### Expression Classes

#### `Expression` (Abstract Base)

- `bool evaluate(const std::set<std::string>& values) const` - Evaluate expression against tags
- `bool operator()(const std::set<std::string>& values) const` - Callable operator
- `std::string to_string() const` - Convert to string representation

#### `Literal`

Represents a single tag in an expression.

```cpp
Literal lit("@fast");
lit.evaluate({"@fast"});  // true
```

#### `And`

Boolean AND operation.

```cpp
std::vector<std::shared_ptr<Expression>> terms = {
    std::make_shared<Literal>("@a"),
    std::make_shared<Literal>("@b")
};
And and_expr(std::move(terms));
and_expr.evaluate({"@a", "@b"});  // true
```

#### `Or`

Boolean OR operation.

```cpp
std::vector<std::shared_ptr<Expression>> terms = {
    std::make_shared<Literal>("@a"),
    std::make_shared<Literal>("@b")
};
Or or_expr(std::move(terms));
or_expr.evaluate({"@a"});  // true
```

#### `Not`

Boolean NOT operation.

```cpp
auto term = std::make_shared<Literal>("@slow");
Not not_expr(term);
not_expr.evaluate({"@fast"});  // true
```

#### `True`

Always evaluates to true (used for empty expressions).

```cpp
True always_true;
always_true.evaluate({});  // true
```

## Grammar

The parser uses the [Shunting Yard algorithm](http://rosettacode.org/wiki/Parsing/Shunting-yard_algorithm) to parse tag expressions.

### Operator Precedence

1. `not` (highest precedence, right associative)
2. `and` (medium precedence, left associative)
3. `or` (lowest precedence, left associative)

### Escaped Characters

Special characters can be escaped with backslash:

- `\\` - Backslash
- `\(` - Open parenthesis
- `\)` - Close parenthesis
- `\ ` - Space

Example:
```cpp
auto expr = parse("tag\\ with\\ spaces");
expr->evaluate({"tag with spaces"});  // true
```

## Test Runner Usage

A gherkin test runner (such as behave or cucumber-cpp) uses tag expressions to select scenarios to run:

```bash
# Select scenarios tagged with both @smoke and @fast
./test_runner --tags="@smoke and @fast"

# Select scenarios tagged with @wip but not @slow
./test_runner --tags="@wip and not @slow"

# Select scenarios tagged with either @login or @registration
./test_runner --tags="@login or @registration"
```

## Comparison with Python Version

This C++ implementation provides feature parity with the official Python implementation while offering:

- **Better Performance**: Compiled C++ code with optimizations
- **Static Typing**: Compile-time type checking
- **Lower Memory Footprint**: Efficient use of standard containers
- **Cross-Platform**: Works on Linux, macOS, and Windows

### API Differences

| Python | C++ |
|--------|-----|
| `parse("@fast")` | `parse("@fast")` |
| `expr({"@fast"})` | `expr->evaluate({"@fast"})` |
| `str(expr)` | `expr->to_string()` |
| `except TagExpressionError` | `catch (const TagExpressionError&)` |

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Based on the [Cucumber Tag Expressions specification](https://cucumber.io/docs/cucumber/api/#tag-expressions)
- Original Python implementation by the Cucumber team
- Uses the Shunting Yard algorithm for parsing

## Related Projects

- [cucumber-tag-expressions (Python)](https://github.com/cucumber/tag-expressions)
- [Cucumber](https://cucumber.io/)
- [Behave](https://behave.readthedocs.io/)

## Support

For issues, questions, or contributions, please visit the [GitHub repository](https://github.com/cucumber/tag-expressions).
