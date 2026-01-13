#ifndef CUCUMBER_TAG_EXPRESSIONS_PARSER_HPP
#define CUCUMBER_TAG_EXPRESSIONS_PARSER_HPP

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "expression.hpp"

namespace cucumber_tag_expressions {

/**
 * @brief Exception thrown by parser if an invalid tag expression is detected.
 */
class TagExpressionError : public std::runtime_error {
public:
    explicit TagExpressionError(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @brief Associativity of boolean operations.
 * 
 * How operators of same precedence are grouped in the absence of parentheses.
 * 
 * - LEFT: Groups `a and b and c` to `(a and b) and c`.
 * - RIGHT: Groups `a or b or c` to `a or (b or c)`.
 */
enum class Associative {
    LEFT,
    RIGHT
};

/**
 * @brief Types of tag expression tokens.
 */
enum class TokenType {
    OPERAND,
    OPERATOR
};

/**
 * @brief Describes tokens and their abilities for tag expression parsing.
 */
enum class Token {
    OR,
    AND,
    NOT,
    OPEN_PARENTHESIS,
    CLOSE_PARENTHESIS
};

/**
 * @brief Token information structure.
 */
struct TokenInfo {
    std::string keyword;
    int precedence;
    Associative assoc;
    TokenType token_type;

    TokenInfo(std::string kw, int prec, Associative a = Associative::LEFT,
              TokenType tt = TokenType::OPERAND)
        : keyword(std::move(kw)), precedence(prec), assoc(a), token_type(tt) {}

    bool is_operation() const { return token_type == TokenType::OPERATOR; }
    bool is_binary() const;
    bool is_unary() const;
    bool has_lower_precedence_than(const TokenInfo& other) const;
    bool matches(const std::string& text) const { return keyword == text; }
};

/**
 * @brief Parser class to parse boolean tag expressions.
 * 
 * Boolean operations:
 * - and (as binary operation: a and b)
 * - or (as binary operation: a or b)
 * - not (as unary operation: not a)
 * 
 * In addition, parentheses can be used to group expressions, like:
 * - "a and (b or c)"
 * - "(a and not b) or (c and d)"
 * 
 * Uses the Shunting Yard algorithm to parse the tag expression.
 */
class TagExpressionParser {
public:
    /**
     * @brief Parse a tag expression as text and return the expression tree.
     * 
     * @param text Tag expression as text to parse
     * @return std::shared_ptr<Expression> Parsed expression tree
     * @throws TagExpressionError If the tag expression is invalid
     */
    static std::shared_ptr<Expression> parse(const std::string& text);

private:
    /**
     * @brief Get token information for a given token enum.
     * 
     * @param token Token enum value
     * @return const TokenInfo& Token information
     */
    static const TokenInfo& get_token_info(Token token);

    /**
     * @brief Select the token that matches the text.
     * 
     * @param text Text to select the matching token
     * @param token Output parameter for the matched token
     * @return true if a token was found
     * @return false otherwise
     */
    static bool select_token(const std::string& text, Token& token);

    /**
     * @brief Creates operand object from parsed text.
     * 
     * @param text Text to create operand from
     * @return std::shared_ptr<Expression> Operand object created from text
     */
    static std::shared_ptr<Expression> make_operand(const std::string& text);

    /**
     * @brief Creates a list of tokens from text.
     * 
     * @param text Tag expression as text to parse
     * @return std::vector<std::string> List of selected tokens
     * @throws TagExpressionError If the tag expression is invalid
     */
    static std::vector<std::string> tokenize(const std::string& text);

    /**
     * @brief Push a new boolean expression on the expression stack.
     * 
     * @param token Token for new expression
     * @param expressions Expression stack to use
     * @throws TagExpressionError If the expression stack contains an unexpected
     *         token or too few operands for an operator
     */
    static void push_expression(Token token,
                                 std::vector<std::shared_ptr<Expression>>& expressions);

    /**
     * @brief Construct a detailed error message for a tag expression error.
     * 
     * @param message Error message to display
     * @param parts List of parts of the tag expression
     * @param error_index Index of the error in the parts list
     * @return std::string Detailed error message with error-position marked
     */
    static std::string make_error_description(const std::string& message,
                                               const std::vector<std::string>& parts,
                                               size_t error_index);
};

/**
 * @brief Convenience function to parse a tag expression as text.
 * 
 * @param text Tag expression as text to parse
 * @return std::shared_ptr<Expression> Parsed expression tree
 * @throws TagExpressionError If the tag expression is invalid
 */
inline std::shared_ptr<Expression> parse(const std::string& text) {
    return TagExpressionParser::parse(text);
}

}  // namespace cucumber_tag_expressions

#endif  // CUCUMBER_TAG_EXPRESSIONS_PARSER_HPP
