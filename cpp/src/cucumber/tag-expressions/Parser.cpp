#include "cucumber/tag-expressions/Parser.hpp"
#include "cucumber/tag-expressions/Error.hpp"
#include "cucumber/tag-expressions/Model.hpp"
#include "cucumber/tag-expressions/Token.hpp"
#include <cctype>
#include <cstddef>
#include <deque>
#include <locale>
#include <memory>
#include <stack>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cucumber::tag_expressions
{
    namespace
    {
        Error SyntaxError(std::string_view expression, std::string_view reason)
        {
            return Error("Tag expression \"" + std::string{ expression } + "\" could not be parsed because of syntax error: " + std::string{ reason } + ".");
        }

        void EnsureExpectedTokenType(std::string_view expression, TokenType tokenType, TokenType expected)
        {
            if (tokenType != expected)
            {
                throw SyntaxError(expression, "Expected " + std::string{ TokenTypeMap().at(expected) });
            }
        }

        void RequireArgCount(std::string_view expression, std::deque<std::unique_ptr<Expression>>& expressions, std::size_t number)
        {
            if (expressions.size() < number)
            {
                throw SyntaxError(expression, "Expected operand");
            }
        }

        template<class T>
        void PushBinary(std::string_view expression, std::deque<std::unique_ptr<Expression>>& expressions)
        {
            RequireArgCount(expression, expressions, 2);

            auto term2 = std::move(expressions.back());
            expressions.pop_back();
            auto term1 = std::move(expressions.back());
            expressions.pop_back();

            expressions.push_back(std::make_unique<T>(std::move(term1), std::move(term2)));
        }

        template<class T>
        void PushUnary(std::string_view expression, std::deque<std::unique_ptr<Expression>>& expressions)
        {
            RequireArgCount(expression, expressions, 1);

            auto term = std::move(expressions.back());
            expressions.pop_back();

            expressions.push_back(std::make_unique<T>(std::move(term)));
        }

        void PushExpression(std::string_view expression, const Token& token, std::deque<std::unique_ptr<Expression>>& expressions)
        {
            if (token == OR)
            {
                PushBinary<OrExpression>(expression, expressions);
            }
            else if (token == AND)
            {
                PushBinary<AndExpression>(expression, expressions);
            }
            else if (token == NOT)
            {
                PushUnary<NotExpression>(expression, expressions);
            }
            else
            {
                throw Error("(Unexpected token: " + std::string{ token.keyword } + ")");
            }
        }

        std::vector<std::string> Tokenize(std::string_view expression)
        {
            std::vector<std::string> tokens{};
            auto escaped = false;
            std::string token = {};

            for (char ch : expression) // NOLINT(readability-identifier-length)
            {
                if (escaped)
                {
                    if ((ch != '(' && ch != ')' && ch != '\\') && !std::isspace(ch, std::locale()))
                    {
                        throw SyntaxError(expression, "Illegal escape before \"" + std::string{ ch } + "\"");
                    }

                    token += ch;
                    escaped = false;
                }
                else if (ch == '\\')
                {
                    escaped = true;
                }
                else if (ch == '(' || ch == ')' || std::isspace(ch, std::locale()))
                {
                    if (!token.empty())
                    {
                        tokens.push_back(std::move(token));
                        token.clear();
                    }
                    if (ch != ' ')
                    {
                        tokens.push_back(std::string{ ch });
                    }
                }
                else
                {
                    token += ch;
                }
            }

            if (!token.empty())
            {
                tokens.push_back(std::move(token));
            }

            return tokens;
        }

        const Token* SelectToken(std::string_view expression)
        {
            const auto& map = TokenMap();

            if (map.find(expression) == map.end())
            {
                return nullptr;
            }

            return &map.at(expression);
        }
    }

    std::unique_ptr<Expression> Parse(std::string_view expression)
    {
        auto tokens = Tokenize(expression);

        if (tokens.empty())
        {
            return std::make_unique<TrueExpression>();
        }

        std::stack<Token> operations;
        std::deque<std::unique_ptr<Expression>> expressions;
        auto expectedTokenType = TokenType::operand;

        for (auto index = 0; index < tokens.size(); ++index)
        {
            const auto& part = tokens[index];

            if (const auto* token = SelectToken(part); token == nullptr)
            {
                EnsureExpectedTokenType(expression, TokenType::operand, expectedTokenType);
                expressions.push_back(std::make_unique<LiteralExpression>(part));
                expectedTokenType = TokenType::operator_;
            }
            else if (*token == NOT || *token == OPEN_PARENTHESIS)
            {
                EnsureExpectedTokenType(expression, TokenType::operand, expectedTokenType);
                operations.push(*token);
                expectedTokenType = TokenType::operand;
            }
            else if (token->IsOperation())
            {
                EnsureExpectedTokenType(expression, TokenType::operator_, expectedTokenType);
                while (!operations.empty() && operations.top().IsOperation() && token->HasLowerPrecedenceThan(operations.top()))
                {
                    auto lastOperation = operations.top();
                    operations.pop();
                    PushExpression(expression, lastOperation, expressions);
                }
                operations.push(*token);
                expectedTokenType = TokenType::operand;
            }
            else if (*token == CLOSE_PARENTHESIS)
            {
                EnsureExpectedTokenType(expression, TokenType::operator_, expectedTokenType);
                while (!operations.empty() && operations.top() != OPEN_PARENTHESIS)
                {
                    auto lastOperation = operations.top();
                    operations.pop();
                    PushExpression(expression, lastOperation, expressions);
                }

                if (operations.empty())
                {
                    throw SyntaxError(expression, "Unmatched )");
                }

                if (operations.top() == OPEN_PARENTHESIS)
                {
                    operations.pop();
                    expectedTokenType = TokenType::operator_;
                }
            }
        }

        while (!operations.empty())
        {
            auto lastOperation = operations.top();
            operations.pop();

            if (lastOperation == OPEN_PARENTHESIS)
            {
                throw SyntaxError(expression, "Unmatched (");
            }

            PushExpression(expression, lastOperation, expressions);
        }

        return std::move(expressions.back());
    }
}
