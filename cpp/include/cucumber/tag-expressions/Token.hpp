#ifndef TAG_EXPRESSION_TOKEN_HPP
#define TAG_EXPRESSION_TOKEN_HPP

#include <cstdint>
#include <map>
#include <optional>
#include <string_view>

namespace cucumber::tag_expressions
{
    enum struct Associative : std::uint8_t
    {
        left,
        right
    };

    enum struct TokenType : std::uint8_t
    {
        operand,
        operator_
    };

    inline const std::map<TokenType, std::string_view>& TokenTypeMap()
    {
        static std::map<TokenType, std::string_view> map{
            { TokenType::operand, "operand" },
            { TokenType::operator_, "operator" },
        };
        return map;
    };

    struct Token
    {
        constexpr Token(std::string_view keyword, std::int32_t precedence, std::optional<Associative> associative, TokenType type = TokenType::operand)
            : keyword{ keyword }
            , precedence{ precedence }
            , associative{ associative }
            , type{ type }
        {}

        [[nodiscard]] bool IsOperation() const;
        [[nodiscard]] bool IsUnary() const;

        [[nodiscard]] bool HasLowerPrecedenceThan(const Token& other) const;

        [[nodiscard]] bool operator==(const Token& other) const;
        [[nodiscard]] bool operator!=(const Token& other) const;

        std::string_view keyword;
        std::int32_t precedence;
        std::optional<Associative> associative;
        TokenType type;
    };

    static constexpr Token OR{ "or", 0, Associative::left, TokenType::operator_ }; // NOLINT(readability-identifier-length)
    static constexpr Token AND{ "and", 1, Associative::left, TokenType::operator_ };
    static constexpr Token NOT{ "not", 2, Associative::right, TokenType::operator_ };
    static constexpr Token OPEN_PARENTHESIS{ "(", -2, std::nullopt };
    static constexpr Token CLOSE_PARENTHESIS{ ")", -1, std::nullopt };

    inline const std::map<std::string_view, const Token&>& TokenMap()
    {
        static std::map<std::string_view, const Token&> map{ { OR.keyword, OR }, { AND.keyword, AND }, { NOT.keyword, NOT }, { OPEN_PARENTHESIS.keyword, OPEN_PARENTHESIS },
            { CLOSE_PARENTHESIS.keyword, CLOSE_PARENTHESIS } };
        return map;
    };
}

#endif
