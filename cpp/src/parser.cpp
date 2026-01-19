#include "cucumber/tag-expressions/parser.hpp"

#include <algorithm>
#include <map>
#include <sstream>

namespace cucumber::tag_expressions {

namespace {

    // Token information map
    inline const std::map<Token, TokenInfo> token_map = {
        {Token::OR, TokenInfo("or", 0, Associative::LEFT, TokenType::OPERATOR)},
        {Token::AND, TokenInfo("and", 1, Associative::LEFT, TokenType::OPERATOR)},
        {Token::NOT, TokenInfo("not", 2, Associative::RIGHT, TokenType::OPERATOR)},
        {Token::OPEN_PARENTHESIS, TokenInfo("(", -2, Associative::LEFT, TokenType::OPERAND)},
        {Token::CLOSE_PARENTHESIS, TokenInfo(")", -1, Associative::LEFT, TokenType::OPERAND)}
    };

}  // namespace

TagExpressionError::TagExpressionError(std::string_view message) :
    std::runtime_error{std::string{message}} {
}

TokenInfo::TokenInfo(std::string kw, int prec, Associative a, TokenType tt) :
    keyword{std::move(kw)},
    precedence{prec},
    assoc{a},
    token_type{tt} {
}

bool TokenInfo::is_operation() const {
    return (token_type == TokenType::OPERATOR);
}

bool TokenInfo::is_binary() const {
    return ((keyword == "or") || (keyword == "and"));
}

bool TokenInfo::is_unary() const {
    return (keyword == "not");
}

bool TokenInfo::has_lower_precedence_than(const TokenInfo& other) const {
    return ((assoc == Associative::LEFT) && (precedence <= other.precedence)) ||
           ((assoc == Associative::RIGHT) && (precedence < other.precedence));
}

bool TokenInfo::matches(std::string_view text) const {
    return keyword == text;
}

const TokenInfo& TagExpressionParser::get_token_info(Token token) {
    auto it = token_map.find(token);
    if (it == token_map.end()) {
        throw TagExpressionError("Unknown token");
    }
    return it->second;
}

bool TagExpressionParser::select_token(std::string_view text, Token& token) {
    for (const auto& [key, value] : token_map) {
        if (value.keyword == text) {
            token = key;
            return true;
        }
    }
    return false;
}

std::unique_ptr<Expression> TagExpressionParser::make_operand(std::string_view text) {
    return std::make_unique<Literal>(std::string{text});
}

std::vector<std::string> TagExpressionParser::tokenize(std::string_view text) {
    std::vector<std::string> tokens;
    bool escaped = false;
    std::string token;

    for (char c : text) {
        if (escaped) {
            if ((c != '(') &&
                (c != ')') &&
                (c != '\\') &&
                !std::isspace(c)) {
                std::ostringstream oss;
                oss << "Tag expression \"" << text
                    << "\" could not be parsed because of syntax error: "
                    << "Illegal escape before \"" << c << "\".";
                throw TagExpressionError(oss.str());
            }
            token += c;
            escaped = false;
        } else if (c == '\\') {
            escaped = true;
        } else if ((c == '(') ||
                   (c == ')') ||
                   std::isspace(c)) {
            if (!token.empty()) {
                tokens.emplace_back(token);
                token.clear();
            }
            if (!std::isspace(c)) {
                tokens.emplace_back(1, c);
            }
        } else {
            token += c;
        }
    }

    if (!token.empty()) {
        tokens.emplace_back(token);
    }

    return tokens;
}

void TagExpressionParser::push_expression(
    Token token, std::vector<std::unique_ptr<Expression>>& expressions) {
    
    auto require_argcount = [&token, &expressions](size_t number) {
        if (expressions.size() < number) {
            const auto& info = get_token_info(token);
            std::ostringstream oss;
            oss << info.keyword << ": Too few operands (need " << number
                << ", have " << expressions.size() << ")";
            throw TagExpressionError(oss.str());
        }
    };

    if (token == Token::OR) {
        require_argcount(2);
        auto right = std::move(expressions.back());
        expressions.pop_back();
        auto left = std::move(expressions.back());
        expressions.pop_back();
        expressions.emplace_back(std::make_unique<Or>(std::move(left), std::move(right)));
    } else if (token == Token::AND) {
        require_argcount(2);
        auto right = std::move(expressions.back());
        expressions.pop_back();
        auto left = std::move(expressions.back());
        expressions.pop_back();
        expressions.emplace_back(std::make_unique<And>(std::move(left), std::move(right)));
    } else if (token == Token::NOT) {
        require_argcount(1);
        auto term = std::move(expressions.back());
        expressions.pop_back();
        expressions.emplace_back(std::make_unique<Not>(std::move(term)));
    } else {
        throw TagExpressionError("Unexpected token");
    }
}

std::string TagExpressionParser::make_error_description(
    std::string_view message, const std::vector<std::string>& parts, size_t error_index) {
    
    if (error_index > parts.size()) {
        error_index = parts.size();
    }

    // Calculate position of error
    size_t good_text_size = 0;
    for (size_t i = 0; i < error_index && i < parts.size(); ++i) {
        if (i > 0) {
            good_text_size += 1;  // Space between tokens
        }
        good_text_size += parts[i].size();
    }

    size_t error_pos = 12 + good_text_size + 1;  // "Expression: " = 12 chars
    std::string underscores(error_pos, '_');

    std::ostringstream expression;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) {
            expression << " ";
        }
        expression << parts[i];
    }

    std::ostringstream oss;
    if (!message.empty()) {
        oss << message << "\n";
    }
    oss << "Expression: " << expression.str() << "\n"
        << underscores << "^ (HERE)";
    
    return oss.str();
}

void TagExpressionParser::ensure_expected_token_type(
    const std::vector<std::string>& parts, TokenType expected, TokenType actual, std::string_view last_part, size_t index) {
    if (expected != actual) {
        std::ostringstream oss;
        std::string expected_name = (expected == TokenType::OPERAND) ? "operand" : "operator";
        oss << "Syntax error. Expected " << expected_name << " after " << last_part;
        std::string message = make_error_description(oss.str(), parts, index);
        throw TagExpressionError(message);
    }
};


void TagExpressionParser::before_push_binary_expression(
    std::stack<Token>& operations, std::vector<std::unique_ptr<Expression>>& expressions, const TokenInfo& token_info) {
    while (!operations.empty()) {
        Token last_operation = operations.top();
        const auto& last_op_info = get_token_info(last_operation);
        if (last_op_info.is_operation() &&
            token_info.has_lower_precedence_than(last_op_info)) {
            operations.pop();
            push_expression(last_operation, expressions);
        } else {
            break;
        }
    }
}

void TagExpressionParser::before_push_close_parenthesis_expression(
    std::stack<Token>& operations, std::vector<std::unique_ptr<Expression>>& expressions) {
    while (operations.top() != Token::OPEN_PARENTHESIS) {
        Token last_operation = operations.top();
        operations.pop();
        push_expression(last_operation, expressions);
    }
}

std::unique_ptr<Expression> TagExpressionParser::parse(std::string_view text) {
    auto parts = tokenize(text);
    
    if (parts.empty()) {
        // Empty tag expression is always true
        return std::make_unique<True>();
    }

    return parse(parts, text);
}

std::unique_ptr<Expression> TagExpressionParser::parse(const std::vector<std::string>& parts, std::string_view text) {
    std::stack<Token> operations;
    std::vector<std::unique_ptr<Expression>> expressions;
    std::string last_part = "BEGIN";
    TokenType expected_token_type = TokenType::OPERAND;

    auto parse_literal_operand = [&](size_t index,
                                     std::string_view part) {
        ensure_expected_token_type(parts, expected_token_type, TokenType::OPERAND, last_part, index);
        expressions.emplace_back(make_operand(part));
        expected_token_type = TokenType::OPERATOR;
    };

    auto parse_binary_token = [&](size_t index,
                                  Token token,
                                  const TokenInfo& token_info) {
        ensure_expected_token_type(parts, expected_token_type, TokenType::OPERATOR, last_part, index);
        before_push_binary_expression(operations, expressions, token_info);
        operations.push(token);
        expected_token_type = TokenType::OPERAND;
    };

    auto parse_unary_token = [&](size_t index,
                                 Token token) {
        ensure_expected_token_type(parts, expected_token_type, TokenType::OPERAND, last_part, index);
        operations.push(token);
        expected_token_type = TokenType::OPERAND;
    };

    auto parse_close_parenthesis_token = [&](size_t index) {
        ensure_expected_token_type(parts, expected_token_type, TokenType::OPERATOR, last_part, index);
        if (operations.empty()) {
            // CASE: TOO FEW OPEN-PARENTHESIS
            std::string message = "Missing '(': Too few open-parens in: " + std::string{text};
            message = make_error_description(message, parts, index);
            throw TagExpressionError(message);
        }

        before_push_close_parenthesis_expression(operations, expressions);
        if (operations.top() == Token::OPEN_PARENTHESIS) {
            operations.pop();
            expected_token_type = TokenType::OPERATOR;
        }
    };

    auto parse_selected_token = [&](size_t index,
                                    Token token) {
        const auto& token_info = get_token_info(token);
        
        if (token_info.is_binary()) {
            // "and" / "or" operator
            parse_binary_token(index, token, token_info);
        } else if (token_info.is_unary() || (token == Token::OPEN_PARENTHESIS)) {
            // "not" / "(" operator
            parse_unary_token(index, token);
        } else if (token == Token::CLOSE_PARENTHESIS) {
            // ")" token
            parse_close_parenthesis_token(index);
        }
    };

    for (size_t index = 0; index < parts.size(); ++index) {
        const auto& part = parts[index];

        if (Token token; !select_token(part, token)) {
            // CASE OPERAND: Literal
            parse_literal_operand(index, part);
        } else {
            parse_selected_token(index, token);
        }
        
        last_part = part;
    }

    // Process remaining operations
    while (!operations.empty()) {
        Token last_operation = operations.top();
        operations.pop();
        
        if (last_operation == Token::OPEN_PARENTHESIS) {
            // CASE: TOO MANY OPEN-PARENTHESIS
            std::string message = "Unclosed '(': Too many open-parens in: " + std::string{text};
            throw TagExpressionError(message);
        }
        
        push_expression(last_operation, expressions);
    }

    // Return boolean tag expression
    if (expressions.size() != 1) {
        throw TagExpressionError("Invalid expression: Expected exactly one result");
    }
    
    return std::move(expressions.back());
}

std::unique_ptr<Expression> parse(std::string_view text) {
    return TagExpressionParser::parse(text);
}

}  // namespace cucumber::tag_expressions
