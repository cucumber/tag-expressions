#include <gtest/gtest.h>
#include "cucumber/tag-expressions/expression.hpp"
#include "cucumber/tag-expressions/parser.hpp"

using namespace cucumber::tag_expressions;

class EvaluationsTest : public ::testing::Test {
protected:
    std::string empty_string = "";
    std::unordered_set<std::string> empty_set{};
};

TEST_F(EvaluationsTest, EmptyTag) {
    auto expr = parse(empty_string);
    EXPECT_TRUE(expr->evaluate(empty_set));
    EXPECT_TRUE(expr->evaluate({"x"}));
    EXPECT_TRUE(expr->evaluate({"y"}));
    EXPECT_TRUE(expr->evaluate({"z"}));
}

TEST_F(EvaluationsTest, TagSpaces) {
    auto expr = parse("   ");
    EXPECT_TRUE(expr->evaluate(empty_set));
    EXPECT_TRUE(expr->evaluate({"x"}));
    EXPECT_TRUE(expr->evaluate({"y"}));
    EXPECT_TRUE(expr->evaluate({"z"}));
}

TEST_F(EvaluationsTest, TagNotX) {
    auto expr = parse("not x");
    EXPECT_TRUE(expr->evaluate(empty_set));
    EXPECT_FALSE(expr->evaluate({"x"}));
    EXPECT_TRUE(expr->evaluate({"y"}));
}

TEST_F(EvaluationsTest, TagNotNotX) {
    auto expr = parse("not not x");
    EXPECT_FALSE(expr->evaluate(empty_set));
    EXPECT_TRUE(expr->evaluate({"x"}));
    EXPECT_FALSE(expr->evaluate({"y"}));
}

TEST_F(EvaluationsTest, TagNotNotNotX) {
    auto expr = parse("not not not x");
    EXPECT_TRUE(expr->evaluate(empty_set));
    EXPECT_FALSE(expr->evaluate({"x"}));
    EXPECT_TRUE(expr->evaluate({"y"}));
}

TEST_F(EvaluationsTest, TagXAndY) {
    auto expr = parse("x and y");
    EXPECT_FALSE(expr->evaluate(empty_set));
    EXPECT_TRUE(expr->evaluate({"x", "y"}));
    EXPECT_FALSE(expr->evaluate({"x"}));
    EXPECT_FALSE(expr->evaluate({"y"}));
}

TEST_F(EvaluationsTest, TagXOrY) {
    auto expr = parse("x or y");
    EXPECT_FALSE(expr->evaluate(empty_set));
    EXPECT_TRUE(expr->evaluate({"x", "y"}));
    EXPECT_TRUE(expr->evaluate({"x"}));
    EXPECT_TRUE(expr->evaluate({"y"}));
}

TEST_F(EvaluationsTest, EscapeParentheses) {
    auto expr = parse("x\\(1\\) or y\\(2\\)");
    EXPECT_FALSE(expr->evaluate(empty_set));
    EXPECT_TRUE(expr->evaluate({"x(1)"}));
    EXPECT_TRUE(expr->evaluate({"y(2)"}));
}
    
TEST_F(EvaluationsTest, EscapeSpecialCharacters1) {
    auto expr = parse("x\\\\ or y\\\\\\) or z\\\\");
    EXPECT_TRUE(expr->evaluate({"x\\"}));
    EXPECT_TRUE(expr->evaluate({"y\\)"}));
    EXPECT_TRUE(expr->evaluate({"z\\"}));
    EXPECT_FALSE(expr->evaluate({"x"}));
    EXPECT_FALSE(expr->evaluate({"y)"}));
    EXPECT_FALSE(expr->evaluate({"z"}));
}    

TEST_F(EvaluationsTest, EscapeSpecialCharacters2) {
    auto expr = parse("\\\\x or y\\\\ or z\\\\");
    EXPECT_TRUE(expr->evaluate({"\\x"}));
    EXPECT_TRUE(expr->evaluate({"y\\"}));
    EXPECT_TRUE(expr->evaluate({"z\\"}));
    EXPECT_FALSE(expr->evaluate({"x"}));
    EXPECT_FALSE(expr->evaluate({"y"}));
    EXPECT_FALSE(expr->evaluate({"z"}));
}
