#include <gtest/gtest.h>
#include "cucumber_tag_expressions/parser.hpp"

using namespace cucumber_tag_expressions;

class ParserTest : public ::testing::Test {
protected:
    std::set<std::string> empty_set{};
};

TEST_F(ParserTest, ParseSimpleLiteral) {
    auto expr = parse("@fast");
    EXPECT_TRUE(expr->evaluate({"@fast"}));
    EXPECT_FALSE(expr->evaluate({"@slow"}));
}

TEST_F(ParserTest, ParseAndExpression) {
    auto expr = parse("@fast and @unit");
    
    EXPECT_TRUE(expr->evaluate({"@fast", "@unit"}));
    EXPECT_FALSE(expr->evaluate({"@fast"}));
    EXPECT_FALSE(expr->evaluate({"@unit"}));
}

TEST_F(ParserTest, ParseOrExpression) {
    auto expr = parse("@fast or @slow");
    
    EXPECT_TRUE(expr->evaluate({"@fast"}));
    EXPECT_TRUE(expr->evaluate({"@slow"}));
    EXPECT_TRUE(expr->evaluate({"@fast", "@slow"}));
    EXPECT_FALSE(expr->evaluate({"@medium"}));
}

TEST_F(ParserTest, ParseNotExpression) {
    auto expr = parse("not @slow");
    
    EXPECT_TRUE(expr->evaluate({"@fast"}));
    EXPECT_TRUE(expr->evaluate(empty_set));
    EXPECT_FALSE(expr->evaluate({"@slow"}));
}

TEST_F(ParserTest, ParseComplexExpression) {
    auto expr = parse("@wip and not @slow");
    
    EXPECT_TRUE(expr->evaluate({"@wip", "@fast"}));
    EXPECT_FALSE(expr->evaluate({"@wip", "@slow"}));
    EXPECT_FALSE(expr->evaluate({"@fast"}));
}

TEST_F(ParserTest, ParseWithParentheses) {
    auto expr = parse("(@fast or @slow) and @unit");
    
    EXPECT_TRUE(expr->evaluate({"@fast", "@unit"}));
    EXPECT_TRUE(expr->evaluate({"@slow", "@unit"}));
    EXPECT_FALSE(expr->evaluate({"@fast"}));
    EXPECT_FALSE(expr->evaluate({"@unit"}));
}

TEST_F(ParserTest, ParseNestedParentheses) {
    auto expr = parse("((@a or @b) and @c) or @d");
    
    EXPECT_TRUE(expr->evaluate({"@a", "@c"}));
    EXPECT_TRUE(expr->evaluate({"@b", "@c"}));
    EXPECT_TRUE(expr->evaluate({"@d"}));
    EXPECT_FALSE(expr->evaluate({"@a"}));
    EXPECT_FALSE(expr->evaluate({"@c"}));
}

TEST_F(ParserTest, ParseEmptyExpression) {
    auto expr = parse("");
    EXPECT_TRUE(expr->evaluate(empty_set));
    EXPECT_TRUE(expr->evaluate({"@anything"}));
}

TEST_F(ParserTest, ParseWithSpaces) {
    auto expr = parse("  @fast   and   @unit  ");
    
    EXPECT_TRUE(expr->evaluate({"@fast", "@unit"}));
    EXPECT_FALSE(expr->evaluate({"@fast"}));
}

TEST_F(ParserTest, ParseEscapedCharacters) {
    auto expr = parse("tag\\ with\\ spaces");
    
    EXPECT_TRUE(expr->evaluate({"tag with spaces"}));
    EXPECT_FALSE(expr->evaluate({"tag"}));
}

TEST_F(ParserTest, ParseEscapedParentheses) {
    auto expr = parse("tag\\(with\\)parens");
    
    EXPECT_TRUE(expr->evaluate({"tag(with)parens"}));
}

TEST_F(ParserTest, ThrowsOnInvalidEscape) {
    EXPECT_THROW(parse("tag\\xtest"), TagExpressionError);
}

TEST_F(ParserTest, ThrowsOnUnbalancedOpenParen) {
    EXPECT_THROW(parse("(@fast and @slow"), TagExpressionError);
}

TEST_F(ParserTest, ThrowsOnUnbalancedCloseParen) {
    EXPECT_THROW(parse("@fast and @slow)"), TagExpressionError);
}

TEST_F(ParserTest, ThrowsOnMissingOperand) {
    EXPECT_THROW(parse("@fast and"), TagExpressionError);
}

TEST_F(ParserTest, ThrowsOnMissingOperator) {
    EXPECT_THROW(parse("@fast @slow"), TagExpressionError);
}

TEST_F(ParserTest, ThrowsOnDoubleOperator) {
    EXPECT_THROW(parse("@fast and and @slow"), TagExpressionError);
}

TEST_F(ParserTest, OperatorPrecedence) {
    // and has higher precedence than or
    auto expr = parse("@a or @b and @c");
    
    // Should be parsed as: @a or (@b and @c)
    EXPECT_TRUE(expr->evaluate({"@a"}));
    EXPECT_TRUE(expr->evaluate({"@b", "@c"}));
    EXPECT_FALSE(expr->evaluate({"@b"}));
    EXPECT_FALSE(expr->evaluate({"@c"}));
}

TEST_F(ParserTest, ParsedExpressionToString) {
    auto expr = parse("@a and @b");
    std::string str = expr->to_string();
    
    // Should contain both tags and 'and'
    EXPECT_NE(str.find("a"), std::string::npos);
    EXPECT_NE(str.find("b"), std::string::npos);
    EXPECT_NE(str.find("and"), std::string::npos);
}

TEST_F(ParserTest, RealWorldExample1) {
    // Example from README
    auto expr = parse("@integration and @fast");
    
    EXPECT_TRUE(expr->evaluate({"@integration", "@fast", "@other"}));
    EXPECT_FALSE(expr->evaluate({"@system", "@fast"}));
}

TEST_F(ParserTest, RealWorldExample2) {
    // Example from README
    auto expr = parse("@login or @registration");
    
    EXPECT_TRUE(expr->evaluate({"@account", "@login"}));
    EXPECT_FALSE(expr->evaluate({"@admin", "@account"}));
}
