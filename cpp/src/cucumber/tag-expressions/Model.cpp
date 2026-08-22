#include "cucumber/tag-expressions/Model.hpp"
#include <cstddef>
#include <functional>
#include <memory>
#include <regex>
#include <set>
#include <string>
#include <string_view>
#include <utility>

namespace cucumber::tag_expressions
{
    bool TrueExpression::Evaluate(const std::set<std::string, std::less<>>& /*tags*/) const
    {
        return true;
    }

    TrueExpression::operator std::string() const
    {
        return "";
    }

    LiteralExpression::LiteralExpression(std::string name)
        : name{ std::move(name) }
    {}

    bool LiteralExpression::Evaluate(const std::set<std::string, std::less<>>& tags) const
    {
        return tags.find(name) != tags.end();
    }

    LiteralExpression::operator std::string() const
    {
        auto replaceAll = [](std::string& str, std::string_view from, std::string_view replaceTo)
        {
            if (from.empty())
            {
                return;
            }

            std::size_t start_pos = 0;

            while ((start_pos = str.find(from, start_pos)) != std::string::npos)
            {
                str.replace(start_pos, from.length(), replaceTo);
                start_pos += replaceTo.length();
            }
        };

        auto copy = name;

        replaceAll(copy, "\\", R"(\\)");
        replaceAll(copy, "(", R"(\()");
        replaceAll(copy, ")", R"(\))");

        copy = std::regex_replace(copy, std::regex(R"((\s))"), R"(\$&)");

        return copy;
    }

    AndExpression::AndExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : left{ std::move(left) }
        , right{ std::move(right) }
    {}

    bool AndExpression::Evaluate(const std::set<std::string, std::less<>>& tags) const
    {
        return left->Evaluate(tags) && right->Evaluate(tags);
    }

    AndExpression::operator std::string() const
    {
        if (!left || !right)
        {
            return "";
        }

        return "( " + static_cast<std::string>(*left) + " and " + static_cast<std::string>(*right) + " )";
    }

    OrExpression::OrExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : left{ std::move(left) }
        , right{ std::move(right) }
    {}

    bool OrExpression::Evaluate(const std::set<std::string, std::less<>>& tags) const
    {
        return left->Evaluate(tags) || right->Evaluate(tags);
    }

    OrExpression::operator std::string() const
    {
        if (!left || !right)
        {
            return "";
        }

        return "( " + static_cast<std::string>(*left) + " or " + static_cast<std::string>(*right) + " )";
    }

    NotExpression::NotExpression(std::unique_ptr<Expression> operand)
        : operand{ std::move(operand) }
    {}

    bool NotExpression::Evaluate(const std::set<std::string, std::less<>>& tags) const
    {
        return !operand->Evaluate(tags);
    }

    NotExpression::operator std::string() const
    {
        if (!operand)
        {
            return "";
        }

        if (const auto& ref = *operand; typeid(ref) == typeid(AndExpression) || typeid(ref) == typeid(OrExpression))
        {
            return "not " + static_cast<std::string>(*operand);
        }

        return "not ( " + static_cast<std::string>(*operand) + " )";
    }
}
