#include "cucumber/tag-expressions/Token.hpp"
#include <tuple>

namespace cucumber::tag_expressions
{
    bool Token::IsOperation() const
    {
        return type == TokenType::operator_;
    }

    bool Token::IsUnary() const
    {
        return *this == NOT;
    }

    bool Token::HasLowerPrecedenceThan(const Token& other) const
    {
        return ((associative.value_or(Associative::right) == Associative::left) && precedence <= other.precedence) || //
               ((associative.value_or(Associative::left) == Associative::right) && precedence < other.precedence);
    }

    bool Token::operator==(const Token& other) const
    {
        return std::tie(keyword, precedence, associative, type) == std::tie(other.keyword, other.precedence, other.associative, other.type);
    }

    bool Token::operator!=(const Token& other) const
    {
        return !(*this == other);
    }
}
