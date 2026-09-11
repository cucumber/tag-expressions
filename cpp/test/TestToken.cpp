#include "cucumber/tag-expressions/Token.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace cucumber::tag_expressions
{
    TEST(TestToken, TestPrecedence)
    {
        EXPECT_THAT(OR.HasLowerPrecedenceThan(OR), testing::IsTrue());
        EXPECT_THAT(OR.HasLowerPrecedenceThan(AND), testing::IsTrue());
        EXPECT_THAT(OR.HasLowerPrecedenceThan(NOT), testing::IsTrue());
        EXPECT_THAT(AND.HasLowerPrecedenceThan(AND), testing::IsTrue());
        EXPECT_THAT(AND.HasLowerPrecedenceThan(OR), testing::IsFalse());
        EXPECT_THAT(AND.HasLowerPrecedenceThan(NOT), testing::IsTrue());
        EXPECT_THAT(NOT.HasLowerPrecedenceThan(NOT), testing::IsFalse());
        EXPECT_THAT(NOT.HasLowerPrecedenceThan(OR), testing::IsFalse());
        EXPECT_THAT(NOT.HasLowerPrecedenceThan(AND), testing::IsFalse());
    }

    TEST(TestToken, TestPrecedenceWithParenthesis)
    {
        EXPECT_THAT(OR.HasLowerPrecedenceThan(OPEN_PARENTHESIS), testing::IsFalse());
        EXPECT_THAT(OR.HasLowerPrecedenceThan(CLOSE_PARENTHESIS), testing::IsFalse());
        EXPECT_THAT(AND.HasLowerPrecedenceThan(OPEN_PARENTHESIS), testing::IsFalse());
        EXPECT_THAT(AND.HasLowerPrecedenceThan(CLOSE_PARENTHESIS), testing::IsFalse());
        EXPECT_THAT(NOT.HasLowerPrecedenceThan(OPEN_PARENTHESIS), testing::IsFalse());
        EXPECT_THAT(NOT.HasLowerPrecedenceThan(CLOSE_PARENTHESIS), testing::IsFalse());
    }
}
