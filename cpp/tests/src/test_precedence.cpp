#include <gtest/gtest.h>
#include "cucumber/tag-expressions/parser.hpp"

using namespace cucumber::tag_expressions;

class PrecedenceTest : public ::testing::Test {
protected:
    const TokenInfo& OR = TagExpressionParser::get_token_info(Token::OR);
    const TokenInfo& AND = TagExpressionParser::get_token_info(Token::AND);
    const TokenInfo& NOT = TagExpressionParser::get_token_info(Token::NOT);
    const TokenInfo& OPEN_PARENTHESIS = TagExpressionParser::get_token_info(Token::OPEN_PARENTHESIS);
    const TokenInfo& CLOSE_PARENTHESIS = TagExpressionParser::get_token_info(Token::CLOSE_PARENTHESIS);
};

TEST_F(PrecedenceTest, TestPrecedenceForTokenOr) {
    EXPECT_TRUE(OR.has_lower_precedence_than(OR));
    EXPECT_TRUE(OR.has_lower_precedence_than(AND));
    EXPECT_TRUE(OR.has_lower_precedence_than(NOT));
    EXPECT_FALSE(OR.has_lower_precedence_than(OPEN_PARENTHESIS));
    EXPECT_FALSE(OR.has_lower_precedence_than(CLOSE_PARENTHESIS));
}

TEST_F(PrecedenceTest, TestPrecedenceForTokenAnd) {
    EXPECT_FALSE(AND.has_lower_precedence_than(OR));
    EXPECT_TRUE(AND.has_lower_precedence_than(AND));
    EXPECT_TRUE(AND.has_lower_precedence_than(NOT));
    EXPECT_FALSE(AND.has_lower_precedence_than(OPEN_PARENTHESIS));
    EXPECT_FALSE(AND.has_lower_precedence_than(CLOSE_PARENTHESIS));
}

TEST_F(PrecedenceTest, TestPrecedenceForTokenNot) {
    EXPECT_FALSE(NOT.has_lower_precedence_than(OR));
    EXPECT_FALSE(NOT.has_lower_precedence_than(AND));
    EXPECT_FALSE(NOT.has_lower_precedence_than(NOT));
    EXPECT_FALSE(NOT.has_lower_precedence_than(OPEN_PARENTHESIS));
    EXPECT_FALSE(NOT.has_lower_precedence_than(CLOSE_PARENTHESIS));
}
