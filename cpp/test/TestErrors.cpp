#include "cucumber/tag-expressions/Error.hpp"
#include "cucumber/tag-expressions/Parser.hpp"
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

        struct ErrorParam
        {
            std::string expression;
            std::string error;
        };

        void PrintTo(const ErrorParam& param, std::ostream* stream)
        {
            *stream << "'" << param.expression << "' => '" << param.error << "'";
        }

        std::vector<ErrorParam> GetErrorParams()
        {
            std::vector<ErrorParam> params;
            const std::filesystem::path testdataPath{ std::filesystem::path{ TESTDATA_SRC } / "errors.yml" };
            const auto testdata = YAML::LoadFile(testdataPath.string());

            for (const auto& node : testdata)
            {
                params.push_back({ node["expression"].as<std::string>(), node["error"].as<std::string>() });
            }

            return params;
        }

        struct TestErrors : testing::TestWithParam<ErrorParam>
        {};
    }

    TEST_P(TestErrors, ReportsSyntaxError)
    {
        const auto& param = GetParam();

        EXPECT_THAT(
            [&param]
            {
                Parse(param.expression);
            },
            testing::ThrowsMessage<Error>(testing::StrEq(param.error)));
    }

    INSTANTIATE_TEST_SUITE_P(FromTestData, TestErrors, testing::ValuesIn(GetErrorParams()),
        [](const testing::TestParamInfo<ErrorParam>& info)
        {
            return Sanitize(info.param.expression) + "_" + std::to_string(info.index);
        });
}
