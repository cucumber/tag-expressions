#include <gtest/gtest.h>
#include "cucumber/tag-expressions/parser.hpp"

using namespace cucumber::tag_expressions;

class ParsingTest : public ::testing::Test {
protected:
    const std::string empty_string = "";
};

TEST_F(ParsingTest, EmptyTag) {
    auto expr = parse(empty_string);
    EXPECT_EQ(expr->to_string(), "");
}

TEST_F(ParsingTest, TagAandB) {
    auto expr = parse("a and b");
    EXPECT_EQ(expr->to_string(), "( a and b )");
}

TEST_F(ParsingTest, TagAorB) {
    auto expr = parse("a or b");
    EXPECT_EQ(expr->to_string(), "( a or b )");
}

TEST_F(ParsingTest, TagNotA) {
    auto expr = parse("not a");
    EXPECT_EQ(expr->to_string(), "not ( a )");
}

TEST_F(ParsingTest, TagAandBandC) {
    auto expr = parse("a and b and c");
    EXPECT_EQ(expr->to_string(), "( ( a and b ) and c )");
}

TEST_F(ParsingTest, TagAandBorCandD) {
    auto expr = parse("(a and b) or (c and d)");
    EXPECT_EQ(expr->to_string(), "( ( a and b ) or ( c and d ) )");
}

TEST_F(ParsingTest, TagNotAorBandNotCorNotDorEandF) {
    auto expr = parse("not a or b and not c or not d or e and f");
    EXPECT_EQ(expr->to_string(), "( ( ( not ( a ) or ( b and not ( c ) ) ) or not ( d ) ) or ( e and f ) )");
}

TEST_F(ParsingTest, TagNotAorBandNotCorNotDorEandFWithEscapes) {
    auto expr = parse(R"(not a\(\) or b and not c or not d or e and f)");
    EXPECT_EQ(expr->to_string(), R"(( ( ( not ( a\(\) ) or ( b and not ( c ) ) ) or not ( d ) ) or ( e and f ) ))");
}


TEST_F(ParsingTest, TagNotAandB) {
    auto expr = parse("not (a and b)");
    EXPECT_EQ(expr->to_string(), "not ( a and b )");
}

TEST_F(ParsingTest, TagNotAorB) {
    auto expr = parse("not (a or b)");
    EXPECT_EQ(expr->to_string(), "not ( a or b )");
}

TEST_F(ParsingTest, TagNotAandBandCorNotDorF) {
    auto expr = parse("not (a and b) and c or not (d or f)");
    EXPECT_EQ(expr->to_string(), "( ( not ( a and b ) and c ) or not ( d or f ) )");
}



TEST_F(ParsingTest, TagAEscapeandB) {
    auto expr = parse(R"(a\\ and b)");
    EXPECT_EQ(expr->to_string(), R"(( a\\ and b ))");
}

TEST_F(ParsingTest, TagEscapeAandB) {
    auto expr = parse(R"(\\a and b)");
    EXPECT_EQ(expr->to_string(), R"(( \\a and b ))");
}

TEST_F(ParsingTest, TagAandBEscape) {
    auto expr = parse(R"(a and b\\)");
    EXPECT_EQ(expr->to_string(), R"(( a and b\\ ))");
}

TEST_F(ParsingTest, TagAandBEscapeEscape) {
    auto expr = parse(R"(a and b\\\\)");
    EXPECT_EQ(expr->to_string(), R"(( a and b\\\\ ))");
}

TEST_F(ParsingTest, TagAEscapeEscapeandBEscapeEscape) {
    auto expr = parse(R"(a\\\( and b\\\))");
    EXPECT_EQ(expr->to_string(), R"(( a\\\( and b\\\) ))");
}

TEST_F(ParsingTest, TagAandEscapeB) {
    auto expr = parse(R"(a and \\b)");
    EXPECT_EQ(expr->to_string(), R"(( a and \\b ))");
}

TEST_F(ParsingTest, TagXorParenthesesY) {
    auto expr = parse("x or(y)");
    EXPECT_EQ(expr->to_string(), "( x or y )");
}

TEST_F(ParsingTest, TagXEscapeEscapeorYEscapeEscape) {
    auto expr = parse(R"(x\(1\) or(y\(2\)))");
    EXPECT_EQ(expr->to_string(), R"(( x\(1\) or y\(2\) ))");
}

TEST_F(ParsingTest, TagEscapeXorYEscapeorZEscape) {
    auto expr = parse(R"(\\x or y\\ or z\\)");
    EXPECT_EQ(expr->to_string(), R"(( ( \\x or y\\ ) or z\\ ))");
}

TEST_F(ParsingTest, TagXEscapeorYEscapeEscapeorZEscape) {
    auto expr = parse(R"(x\\ or(y\\\)) or(z\\))");
    EXPECT_EQ(expr->to_string(), R"(( ( x\\ or y\\\) ) or z\\ ))");
}

TEST_F(ParsingTest, TagXEscapeorY) {
    auto expr = parse(R"(x\  or y)");
    EXPECT_EQ(expr->to_string(), R"(( x\  or y ))");
}

TEST_F(ParsingTest, TagNotNotA) {
    auto expr = parse("not not a");
    EXPECT_EQ(expr->to_string(), "not ( not ( a ) )");
}

TEST_F(ParsingTest, TagNotNotNotA) {
    auto expr = parse("not not not a");
    EXPECT_EQ(expr->to_string(), "not ( not ( not ( a ) ) )");
}
