#include "cucumber/tag-expressions/Parser.hpp"
#include "yaml-cpp/yaml.h"
#include <cctype>
#include <cstddef>
#include <filesystem>
#include <functional>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <ostream>
#include <set>
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
                result += std::isalnum(static_cast<unsigned char>(c)) ? c : '_';
            return result.empty() ? std::string{ "empty" } : result;
        }

        struct EvaluationParam
        {
            std::string expression;
            std::vector<std::string> variables;
            bool result;
        };

        // Determines how Google Test prints the parameter value.
        void PrintTo(const EvaluationParam& param, std::ostream* stream)
        {
            *stream << "'" << param.expression << "' [";
            for (std::size_t i = 0; i < param.variables.size(); ++i)
                *stream << (i == 0 ? "" : ", ") << param.variables[i];
            *stream << "] => " << (param.result ? "true" : "false");
        }

        std::vector<EvaluationParam> GetEvaluationParams()
        {
            std::vector<EvaluationParam> params;
            const std::filesystem::path testdataPath{ std::filesystem::path{ TESTDATA_SRC } / "evaluations.yml" };
            const auto testdata = YAML::LoadFile(testdataPath.string());

            for (const auto& node : testdata)
                for (const auto& test : node["tests"])
                    params.push_back({ node["expression"].as<std::string>(), test["variables"].as<std::vector<std::string>>(), test["result"].as<bool>() });

            return params;
        }
    }

    struct TestEvaluations : testing::TestWithParam<EvaluationParam>
    {};

    TEST_P(TestEvaluations, EvaluatesExpression)
    {
        const auto& param = GetParam();

        const auto tagExpression = Parse(param.expression);
        ASSERT_THAT(tagExpression, testing::NotNull());

        const std::set<std::string, std::less<>> tags(param.variables.begin(), param.variables.end());

        EXPECT_THAT(tagExpression->Evaluate(tags), testing::Eq(param.result));
    }

    INSTANTIATE_TEST_SUITE_P(FromTestData, TestEvaluations, testing::ValuesIn(GetEvaluationParams()),
        [](const testing::TestParamInfo<EvaluationParam>& info)
        {
            std::string name = Sanitize(info.param.expression);
            for (const auto& variable : info.param.variables)
                name += "_" + Sanitize(variable);
            name += info.param.result ? "_true" : "_false";
            return name + "_" + std::to_string(info.index);
        });
}
