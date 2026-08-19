#ifndef CUCUMBER_TAG_EXPRESSIONS_PARSER_HPP_
#define CUCUMBER_TAG_EXPRESSIONS_PARSER_HPP_

#include <memory>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

#include "expression.hpp"

class PrecedenceTest;

namespace cucumber::tag_expressions {

    /**
     * @brief Exception thrown by parser if an invalid tag expression is detected.
     */
    class TagExpressionError : public std::runtime_error {
    public:
        explicit TagExpressionError(std::string_view message);
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

        TokenInfo(std::string kw, int prec, Associative a = Associative::LEFT, TokenType tt = TokenType::OPERAND);

        bool is_operation() const;
        bool is_binary() const;
        bool is_unary() const;
        bool has_lower_precedence_than(const TokenInfo& other) const;
        bool matches(std::string_view text) const;
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
         * @return std::unique_ptr<Expression> Parsed expression tree
         * @throws TagExpressionError If the tag expression is invalid
         */
        static std::unique_ptr<Expression> parse(std::string_view text);

    private:
        /**
         * @brief Parses a tag expression from the given parts and text.
         *
         * @param parts A vector of string parts that make up the tag expression to be parsed.
         * @param text A string view of the original text being parsed.
         * @return A unique pointer to the parsed Expression object.
         */
        static std::unique_ptr<Expression> parse(const std::vector<std::string>& parts, std::string_view text);

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
        static bool select_token(std::string_view text, Token& token);

        /**
         * @brief Creates operand object from parsed text.
         * 
         * @param text Text to create operand from
         * @return std::unique_ptr<Expression> Operand object created from text
         */
        static std::unique_ptr<Expression> make_operand(std::string_view text);

        /**
         * @brief Creates a list of tokens from text.
         * 
         * @param text Tag expression as text to parse
         * @return std::vector<std::string> List of selected tokens
         * @throws TagExpressionError If the tag expression is invalid
         */
        static std::vector<std::string> tokenize(std::string_view text);

        /**
         * @brief Push a new boolean expression on the expression stack.
         * 
         * @param token Token for new expression
         * @param expressions Expression stack to use
         * @throws TagExpressionError If the expression stack contains an unexpected
         *         token or too few operands for an operator
         */
        static void push_expression(Token token,
                                    std::vector<std::unique_ptr<Expression>>& expressions);

        /**
         * @brief Construct a detailed error message for a tag expression error.
         * 
         * @param message Error message to display
         * @param parts List of parts of the tag expression
         * @param error_index Index of the error in the parts list
         * @return std::string Detailed error message with error-position marked
         */
        static std::string make_error_description(std::string_view message,
                                                  const std::vector<std::string>& parts,
                                                  size_t error_index);

        /**
         * @brief Ensures that a token has the expected type.
         * 
         * Validates that a token at a given position in the parts list matches the expected token type.
         * If the actual token type does not match the expected type, an exception or error is raised.
         * 
         * @param parts A vector of string parts that make up the parsed expression.
         * @param expected The expected TokenType for validation.
         * @param actual The actual TokenType found at the specified index.
         * @param last_part A string view representing the last parsed part of the expression.
         * @param index The position in the parts vector where the token validation occurs.
         * 
         * @throws std::exception If the actual token type does not match the expected token type.
         */
        static void ensure_expected_token_type(const std::vector<std::string>& parts,
                                               TokenType expected,
                                               TokenType actual,
                                               std::string_view last_part,
                                               size_t index);

        /**
         * @brief Handles the preparation of a binary expression before pushing it onto the operations stack.
         *
         * This static method manages the precedence and associativity of binary operators in tag expression parsing.
         * It processes any pending operations with higher or equal precedence before adding a new operator to the stack.
         *
         * @param operations A stack of tokens representing pending operations to be processed.
         * @param expressions A vector of unique pointers to Expression objects that make up the expression tree.
         * @param token_info Information about the current token being processed, including its type and value.
         *
         * @note This method is part of the Shunting Yard algorithm implementation for expression parsing.
         * @note The method ensures correct operator precedence and handles the conversion of infix notation to an expression tree.
         */
        static void before_push_binary_expression(std::stack<Token>& operations,
                                                  std::vector<std::unique_ptr<Expression>>& expressions,
                                                  const TokenInfo& token_info);

        /**
         * @brief Processes operations and expressions when a closing parenthesis is encountered.
         * 
         * This static method handles the parsing logic that occurs before pushing a close parenthesis
         * token onto the operations stack. It manages the precedence and associativity of operators
         * by popping operations from the stack and converting them into expression nodes.
         * 
         * @param operations A stack of tokens representing pending operations to be processed.
         *                   Operations are popped and converted to expressions based on precedence rules.
         * @param expressions A vector of unique_ptr to Expression objects that accumulate the
         *                    resulting expression tree nodes.
         * 
         * @note This method is typically called during parsing when a closing parenthesis is encountered,
         *       ensuring that all operations within the parentheses are properly evaluated and converted
         *       to their corresponding expression objects before closing the parenthetical scope.
         */
        static void before_push_close_parenthesis_expression(std::stack<Token>& operations,
                                                             std::vector<std::unique_ptr<Expression>>& expressions);
        /**
         * @brief Convenience function to access private members for testing.
         */
        friend class ::PrecedenceTest;
    };

    /**
     * @brief Convenience function to parse a tag expression as text.
     * 
     * @param text Tag expression as text to parse
     * @return std::unique_ptr<Expression> Parsed expression tree
     * @throws TagExpressionError If the tag expression is invalid
     */
    std::unique_ptr<Expression> parse(std::string_view text);

}  // namespace cucumber::tag_expressions

#endif  // CUCUMBER_TAG_EXPRESSIONS_PARSER_HPP_
