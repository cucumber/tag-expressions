#include "cucumber/tag-expressions/Parser.hpp"
#include "yaml-cpp/node/parse.h"
#include "yaml-cpp/yaml.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <cctype>
#include <filesystem>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

namespace cucumber::tag_expressions
{
    namespace
    {
        std::string Sanitize(std::string_view text)
        {
            std::string result;
            for (const char c : text)
            {
                result += std::isalnum(static_cast<unsigned char>(c)) ? c : '_';
            }
            return result.empty() ? std::string{ "empty" } : result;
        }

        struct ParsingParam
        {
            std::string expression;
            std::string formatted;
        };

        void PrintTo(const ParsingParam& param, std::ostream* stream)
        {
            *stream << "'" << param.expression << "' => '" << param.formatted << "'";
        }

        std::vector<ParsingParam> GetParsingParams()
        {
            std::vector<ParsingParam> params;
            const std::filesystem::path testdataPath{ std::filesystem::path{ TESTDATA_SRC } / "parsing.yml" };
            const auto testdata = YAML::LoadFile(testdataPath.string());

            for (const auto& node : testdata)
            {
                params.push_back({ node["expression"].as<std::string>(), node["formatted"].as<std::string>() });
            }

            return params;
        }

        struct TestParsing : testing::TestWithParam<ParsingParam>
        {};
    }

    TEST_P(TestParsing, FormatsExpression)
    {
        const auto& param = GetParam();

        const auto tagExpression = Parse(param.expression);
        ASSERT_THAT(tagExpression, testing::NotNull());

        const auto actualText = static_cast<std::string>(*tagExpression);
        EXPECT_THAT(actualText, testing::StrEq(param.formatted));
    }

    INSTANTIATE_TEST_SUITE_P(FromTestData, TestParsing, testing::ValuesIn(GetParsingParams()),
        [](const testing::TestParamInfo<ParsingParam>& info)
        {
            return Sanitize(info.param.expression) + "_" + std::to_string(info.index);
        });
}
