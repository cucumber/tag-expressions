#include <gtest/gtest.h>
#include "cucumber/tag-expressions/parser.hpp"

using namespace cucumber::tag_expressions;

class ErrorsTest : public ::testing::Test {
};

TEST_F(ErrorsTest, ThrowsOnWrongOperatorUsage1) {
    // error: 'Tag expression "@a @b or" could not be parsed because of syntax error: Expected operator.'
    EXPECT_THROW(parse("@a @b or"), TagExpressionError);
}

TEST_F(ErrorsTest, ThrowsOnWrongOperatorUsage2) {
    // error: 'Tag expression "@a and (@b not)" could not be parsed because of syntax error: Expected operator.'
    EXPECT_THROW(parse("@a and (@b not)"), TagExpressionError);
}

TEST_F(ErrorsTest, ThrowsOnWrongOperatorUsage3) {
    // error: 'Tag expression "@a and (@b @c) or" could not be parsed because of syntax error: Expected operator.'
    EXPECT_THROW(parse("@a and (@b @c) or"), TagExpressionError);
}

TEST_F(ErrorsTest, ThrowsOnDoubleOperator1) {
    // error: 'Tag expression "@a and or" could not be parsed because of syntax error: Expected operand.'
    EXPECT_THROW(parse("@a and or"), TagExpressionError);
}

TEST_F(ErrorsTest, ThrowsOnDoubleOperator2) {
    // error: 'Tag expression "or or" could not be parsed because of syntax error: Expected operand.'
    EXPECT_THROW(parse("or or"), TagExpressionError);
}

TEST_F(ErrorsTest, ThrowsOnDoubleOperator3) {
    // error: 'Tag expression "a and or" could not be parsed because of syntax error: Expected operand.'
    EXPECT_THROW(parse("a and or"), TagExpressionError);
}

TEST_F(ErrorsTest, ThrowsOnMissingOperatorBetweenTags) {
    // error: 'Tag expression "a b" could not be parsed because of syntax error: Expected operator.'
    EXPECT_THROW(parse("a b"), TagExpressionError);
}

TEST_F(ErrorsTest, ThrowsOnUnbalancedCloseParentheses) {
    // error: 'Tag expression "( a and b ) )" could not be parsed because of syntax error: Unmatched ).'
    EXPECT_THROW(parse("( a and b ) )"), TagExpressionError);
}

TEST_F(ErrorsTest, ThrowsOnUnbalancedOpenParentheses) {
    // error: 'Tag expression "( ( a and b )" could not be parsed because of syntax error: Unmatched (.'
    EXPECT_THROW(parse("( ( a and b )"), TagExpressionError);
}

TEST_F(ErrorsTest, ThrowsOnEscapeRegularCharacter) {
    // error: 'Tag expression "x or \y or z" could not be parsed because of syntax error: Invalid escape before "y".'
    EXPECT_THROW(parse("x or \\y or z"), TagExpressionError);
}

TEST_F(ErrorsTest, ThrowsOnEscapeNothing) {
    // error: 'Tag expression "x\ or y" could not be parsed because of syntax error: Expected operator.'
    EXPECT_THROW(parse("x\\ or y"), TagExpressionError);
}

TEST_F(ErrorsTest, ThrowsOnAndMissingRightOperand) {
    // error: 'Tag expression "a and" could not be parsed because of syntax error: Expected operand.'
    EXPECT_THROW(parse("a and"), TagExpressionError);
}

TEST_F(ErrorsTest, ThrowsOnOrMissingRightOperand) {
    // error: 'Tag expression "a or" could not be parsed because of syntax error: Expected operand.'
    EXPECT_THROW(parse("a or"), TagExpressionError);
}

TEST_F(ErrorsTest, ThrowsOnNotMissingRightOperand1) {
    // error: 'Tag expression "not" could not be parsed because of syntax error: Expected operand.'
    EXPECT_THROW(parse("not"), TagExpressionError);
}

TEST_F(ErrorsTest, ThrowsOnNotMissingRightOperand2) {
    // error: 'Tag expression "a and not" could not be parsed because of syntax error: Expected operand.'
    EXPECT_THROW(parse("a and not"), TagExpressionError);
}

TEST_F(ErrorsTest, ThrowsOnAndMissingLeftOperand) {
    // error: 'Tag expression "and a" could not be parsed because of syntax error: Expected operand.'
    EXPECT_THROW(parse("and a"), TagExpressionError);
}

TEST_F(ErrorsTest, ThrowsOnOrMissingLeftOperand) {
    // error: 'Tag expression "or a" could not be parsed because of syntax error: Expected operand.'
    EXPECT_THROW(parse("or a"), TagExpressionError);
}
