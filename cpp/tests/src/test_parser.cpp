#include <gtest/gtest.h>
#include "cucumber/tag-expressions/parser.hpp"

using namespace cucumber::tag_expressions;

class ExpressionTest : public ::testing::Test {
protected:
    const std::vector<std::string> empty_set{};
};

TEST_F(ExpressionTest, LiteralEvaluatesCorrectly) {
    Literal literal("@fast");
    
    EXPECT_TRUE(literal.evaluate({"@fast"}));
    EXPECT_TRUE(literal.evaluate({"@fast", "@wip"}));
    EXPECT_FALSE(literal.evaluate({"@slow"}));
    EXPECT_FALSE(literal.evaluate(empty_set));
}

TEST_F(ExpressionTest, LiteralName) {
    Literal literal("@fast");
    EXPECT_EQ("@fast", literal.name());
}

TEST_F(ExpressionTest, LiteralToString) {
    Literal literal("@fast");
    EXPECT_EQ("@fast", literal.to_string());
}

TEST_F(ExpressionTest, LiteralEscapesSpecialCharacters) {
    Literal literal("tag with spaces");
    std::string str = literal.to_string();
    EXPECT_NE(str.find("\\"), std::string::npos);
    EXPECT_EQ(R"(tag\ with\ spaces)", str);
}

TEST_F(ExpressionTest, AndEvaluatesCorrectly) {
    auto term1 = std::make_unique<Literal>("a");
    auto term2 = std::make_unique<Literal>("b");
    And and_expr(std::move(term1), std::move(term2));
    
    EXPECT_TRUE(and_expr.evaluate({"a", "b"}));
    EXPECT_TRUE(and_expr.evaluate({"a", "b", "c"}));
    EXPECT_FALSE(and_expr.evaluate({"a"}));
    EXPECT_FALSE(and_expr.evaluate({"b"}));
    EXPECT_FALSE(and_expr.evaluate(empty_set));
    EXPECT_TRUE(and_expr.left()->evaluate({"a"}));
    EXPECT_FALSE(and_expr.right()->evaluate({"a"}));
    EXPECT_FALSE(and_expr.left()->evaluate({"b"}));
    EXPECT_TRUE(and_expr.right()->evaluate({"b"}));
}

TEST_F(ExpressionTest, AndToString) {
    auto term1 = std::make_unique<Literal>("a");
    auto term2 = std::make_unique<Literal>("b");
    And and_expr(std::move(term1), std::move(term2));
    
    EXPECT_EQ("( a and b )", and_expr.to_string());
    EXPECT_EQ("a", and_expr.left()->to_string());
    EXPECT_EQ("b", and_expr.right()->to_string());
}

TEST_F(ExpressionTest, OrEvaluatesCorrectly) {
    auto term1 = std::make_unique<Literal>("a");
    auto term2 = std::make_unique<Literal>("b");
    Or or_expr(std::move(term1), std::move(term2));
    
    EXPECT_TRUE(or_expr.evaluate({"a", "b"}));
    EXPECT_TRUE(or_expr.evaluate({"a"}));
    EXPECT_TRUE(or_expr.evaluate({"b"}));
    EXPECT_FALSE(or_expr.evaluate({"c"}));
    EXPECT_FALSE(or_expr.evaluate(empty_set));
    EXPECT_TRUE(or_expr.left()->evaluate({"a"}));
    EXPECT_FALSE(or_expr.right()->evaluate({"a"}));
    EXPECT_FALSE(or_expr.left()->evaluate({"b"}));
    EXPECT_TRUE(or_expr.right()->evaluate({"b"}));
}

TEST_F(ExpressionTest, OrToString) {
    auto term1 = std::make_unique<Literal>("a");
    auto term2 = std::make_unique<Literal>("b");
    Or or_expr(std::move(term1), std::move(term2));
    
    EXPECT_EQ("( a or b )", or_expr.to_string());
    EXPECT_EQ("a", or_expr.left()->to_string());
    EXPECT_EQ("b", or_expr.right()->to_string());
}

TEST_F(ExpressionTest, NotEvaluatesCorrectly) {
    auto term = std::make_unique<Literal>("a");
    Not not_expr(std::move(term));
    
    EXPECT_FALSE(not_expr.evaluate({"a"}));
    EXPECT_TRUE(not_expr.evaluate({"b"}));
    EXPECT_TRUE(not_expr.evaluate(empty_set));
    EXPECT_TRUE(not_expr.term()->evaluate({"a"}));
    EXPECT_FALSE(not_expr.term()->evaluate({"b"}));
}

TEST_F(ExpressionTest, NotToString) {
    auto term = std::make_unique<Literal>("a");
    Not not_expr(std::move(term));
    
    EXPECT_EQ("not ( a )", not_expr.to_string());
    EXPECT_EQ("a", not_expr.term()->to_string());
}

TEST_F(ExpressionTest, TrueAlwaysEvaluatesTrue) {
    True true_expr;
    
    EXPECT_TRUE(true_expr.evaluate({"a"}));
    EXPECT_TRUE(true_expr.evaluate(empty_set));
}

TEST_F(ExpressionTest, ComplexExpression) {
    // (a or b) and c
    auto lit_a = std::make_unique<Literal>("a");
    auto lit_b = std::make_unique<Literal>("b");
    auto lit_c = std::make_unique<Literal>("c");

    auto or_expr = std::make_unique<Or>(std::move(lit_a), std::move(lit_b));
    And and_expr(std::move(or_expr), std::move(lit_c));

    EXPECT_TRUE(and_expr.evaluate({"a", "c"}));
    EXPECT_TRUE(and_expr.evaluate({"b", "c"}));
    EXPECT_FALSE(and_expr.evaluate({"a", "b"}));
    EXPECT_FALSE(and_expr.evaluate({"c"}));
    EXPECT_FALSE(and_expr.evaluate(empty_set));
}

TEST_F(ExpressionTest, CallOperatorWorks) {
    Literal literal("@fast");
    
    EXPECT_TRUE(literal({"@fast"}));
    EXPECT_FALSE(literal({"@slow"}));
}

TEST_F(ExpressionTest, TrueToString) {
    True true_expr;
    EXPECT_EQ("", true_expr.to_string());
}
