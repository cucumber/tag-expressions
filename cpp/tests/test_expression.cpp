#include <gtest/gtest.h>
#include "cucumber_tag_expressions/expression.hpp"

using namespace cucumber_tag_expressions;

class ExpressionTest : public ::testing::Test {
protected:
    std::set<std::string> empty_set{};
};

TEST_F(ExpressionTest, LiteralEvaluatesCorrectly) {
    Literal literal("@fast");
    
    EXPECT_TRUE(literal.evaluate({"@fast"}));
    EXPECT_TRUE(literal.evaluate({"@fast", "@wip"}));
    EXPECT_FALSE(literal.evaluate({"@slow"}));
    EXPECT_FALSE(literal.evaluate(empty_set));
}

TEST_F(ExpressionTest, LiteralToString) {
    Literal literal("@fast");
    EXPECT_EQ("@fast", literal.to_string());
}

TEST_F(ExpressionTest, LiteralEscapesSpecialCharacters) {
    Literal literal("tag with spaces");
    std::string str = literal.to_string();
    EXPECT_NE(str.find("\\"), std::string::npos);
}

TEST_F(ExpressionTest, AndEvaluatesCorrectly) {
    auto term1 = std::make_shared<Literal>("a");
    auto term2 = std::make_shared<Literal>("b");
    std::vector<std::shared_ptr<Expression>> terms = {term1, term2};
    And and_expr(std::move(terms));
    
    EXPECT_TRUE(and_expr.evaluate({"a", "b"}));
    EXPECT_TRUE(and_expr.evaluate({"a", "b", "c"}));
    EXPECT_FALSE(and_expr.evaluate({"a"}));
    EXPECT_FALSE(and_expr.evaluate({"b"}));
    EXPECT_FALSE(and_expr.evaluate(empty_set));
}

TEST_F(ExpressionTest, AndToString) {
    auto term1 = std::make_shared<Literal>("a");
    auto term2 = std::make_shared<Literal>("b");
    std::vector<std::shared_ptr<Expression>> terms = {term1, term2};
    And and_expr(std::move(terms));
    
    EXPECT_EQ("( a and b )", and_expr.to_string());
}

TEST_F(ExpressionTest, OrEvaluatesCorrectly) {
    auto term1 = std::make_shared<Literal>("a");
    auto term2 = std::make_shared<Literal>("b");
    std::vector<std::shared_ptr<Expression>> terms = {term1, term2};
    Or or_expr(std::move(terms));
    
    EXPECT_TRUE(or_expr.evaluate({"a", "b"}));
    EXPECT_TRUE(or_expr.evaluate({"a"}));
    EXPECT_TRUE(or_expr.evaluate({"b"}));
    EXPECT_FALSE(or_expr.evaluate({"c"}));
    EXPECT_FALSE(or_expr.evaluate(empty_set));
}

TEST_F(ExpressionTest, OrToString) {
    auto term1 = std::make_shared<Literal>("a");
    auto term2 = std::make_shared<Literal>("b");
    std::vector<std::shared_ptr<Expression>> terms = {term1, term2};
    Or or_expr(std::move(terms));
    
    EXPECT_EQ("( a or b )", or_expr.to_string());
}

TEST_F(ExpressionTest, NotEvaluatesCorrectly) {
    auto term = std::make_shared<Literal>("a");
    Not not_expr(term);
    
    EXPECT_FALSE(not_expr.evaluate({"a"}));
    EXPECT_TRUE(not_expr.evaluate({"b"}));
    EXPECT_TRUE(not_expr.evaluate(empty_set));
}

TEST_F(ExpressionTest, NotToString) {
    auto term = std::make_shared<Literal>("a");
    Not not_expr(term);
    
    EXPECT_EQ("not ( a )", not_expr.to_string());
}

TEST_F(ExpressionTest, TrueAlwaysEvaluatesTrue) {
    True true_expr;
    
    EXPECT_TRUE(true_expr.evaluate({"a"}));
    EXPECT_TRUE(true_expr.evaluate(empty_set));
}

TEST_F(ExpressionTest, ComplexExpression) {
    // (a or b) and c
    auto lit_a = std::make_shared<Literal>("a");
    auto lit_b = std::make_shared<Literal>("b");
    auto lit_c = std::make_shared<Literal>("c");
    
    std::vector<std::shared_ptr<Expression>> or_terms = {lit_a, lit_b};
    auto or_expr = std::make_shared<Or>(std::move(or_terms));
    
    std::vector<std::shared_ptr<Expression>> and_terms = {or_expr, lit_c};
    And and_expr(std::move(and_terms));
    
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
