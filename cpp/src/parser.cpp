#include "cucumber/tag-expressions/parser.hpp"

#include <algorithm>
#include <map>
#include <sstream>
#include <stack>

namespace cucumber::tag_expressions {

namespace {

    // Token information map
    const std::map<cucumber::tag_expressions::Token, cucumber::tag_expressions::TokenInfo>& get_token_map() {
        using namespace cucumber::tag_expressions;
        static const std::map<Token, TokenInfo> map = {
            {Token::OR, TokenInfo("or", 0, Associative::LEFT, TokenType::OPERATOR)},
            {Token::AND, TokenInfo("and", 1, Associative::LEFT, TokenType::OPERATOR)},
            {Token::NOT, TokenInfo("not", 2, Associative::RIGHT, TokenType::OPERATOR)},
            {Token::OPEN_PARENTHESIS, TokenInfo("(", -2, Associative::LEFT, TokenType::OPERAND)},
            {Token::CLOSE_PARENTHESIS, TokenInfo(")", -1, Associative::LEFT, TokenType::OPERAND)}
        };
        return map;
    }

}  // namespace

TagExpressionError::TagExpressionError(const std::string& message) :
    std::runtime_error{message} {
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

bool TokenInfo::matches(const std::string& text) const {
    return keyword == text;
}

const TokenInfo& TagExpressionParser::get_token_info(Token token) {
    const auto& map = get_token_map();
    auto it = map.find(token);
    if (it == map.end()) {
        throw TagExpressionError("Unknown token");
    }
    return it->second;
}

bool TagExpressionParser::select_token(const std::string& text, Token& token) {
    const auto& map = get_token_map();
    for (const auto& pair : map) {
        if (pair.second.keyword == text) {
            token = pair.first;
            return true;
        }
    }
    return false;
}

std::unique_ptr<Expression> TagExpressionParser::make_operand(const std::string& text) {
    return std::make_unique<Literal>(text);
}

std::vector<std::string> TagExpressionParser::tokenize(const std::string& text) {
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
                tokens.push_back(token);
                token.clear();
            }
            if (!std::isspace(c)) {
                tokens.push_back(std::string(1, c));
            }
        } else {
            token += c;
        }
    }

    if (!token.empty()) {
        tokens.push_back(token);
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
        expressions.push_back(std::make_unique<Or>(std::move(left), std::move(right)));
    } else if (token == Token::AND) {
        require_argcount(2);
        auto right = std::move(expressions.back());
        expressions.pop_back();
        auto left = std::move(expressions.back());
        expressions.pop_back();
        expressions.push_back(std::make_unique<And>(std::move(left), std::move(right)));
    } else if (token == Token::NOT) {
        require_argcount(1);
        auto term = std::move(expressions.back());
        expressions.pop_back();
        expressions.push_back(std::make_unique<Not>(std::move(term)));
    } else {
        throw TagExpressionError("Unexpected token");
    }
}

std::string TagExpressionParser::make_error_description(
    const std::string& message, const std::vector<std::string>& parts, size_t error_index) {
    
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

std::unique_ptr<Expression> TagExpressionParser::parse(const std::string& text) {
    auto parts = tokenize(text);
    
    if (parts.empty()) {
        // Empty tag expression is always true
        return std::make_unique<True>();
    }

    auto ensure_expected_token_type = [&](TokenType expected,
                                          TokenType actual,
                                          const std::string& last_part,
                                          size_t index) {
        if (expected != actual) {
            std::string expected_name = (expected == TokenType::OPERAND) ? "operand" : "operator";
            std::ostringstream oss;
            oss << "Syntax error. Expected " << expected_name << " after " << last_part;
            std::string message = make_error_description(oss.str(), parts, index);
            throw TagExpressionError(message);
        }
    };

    std::stack<Token> operations;
    std::vector<std::unique_ptr<Expression>> expressions;
    std::string last_part = "BEGIN";
    TokenType expected_token_type = TokenType::OPERAND;

    for (size_t index = 0; index < parts.size(); ++index) {
        const auto& part = parts[index];
        Token token;
        
        if (!select_token(part, token)) {
            // CASE OPERAND: Literal
            ensure_expected_token_type(expected_token_type, TokenType::OPERAND, last_part, index);
            expressions.push_back(make_operand(part));
            expected_token_type = TokenType::OPERATOR;
        } else {
            const auto& token_info = get_token_info(token);
            
            if (token_info.is_unary()) {
                // "not" operator
                ensure_expected_token_type(expected_token_type, TokenType::OPERAND, last_part, index);
                operations.push(token);
                expected_token_type = TokenType::OPERAND;
            } else if (token_info.is_binary()) {
                // "and" / "or" operator
                ensure_expected_token_type(expected_token_type, TokenType::OPERATOR, last_part, index);
                
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
                
                operations.push(token);
                expected_token_type = TokenType::OPERAND;
            } else if (token == Token::OPEN_PARENTHESIS) {
                // "(" token
                ensure_expected_token_type(expected_token_type, TokenType::OPERAND, last_part, index);
                operations.push(token);
                expected_token_type = TokenType::OPERAND;
            } else if (token == Token::CLOSE_PARENTHESIS) {
                // ")" token
                ensure_expected_token_type(expected_token_type, TokenType::OPERATOR, last_part, index);

                if (operations.empty()) {
                    // CASE: TOO FEW OPEN-PARENTHESIS
                    std::string message = "Missing '(': Too few open-parens in: " + text;
                    message = make_error_description(message, parts, index);
                    throw TagExpressionError(message);
                }
                
                while (operations.top() != Token::OPEN_PARENTHESIS) {
                    Token last_operation = operations.top();
                    operations.pop();
                    push_expression(last_operation, expressions);
                }
                
                if (operations.top() == Token::OPEN_PARENTHESIS) {
                    operations.pop();
                    expected_token_type = TokenType::OPERATOR;
                }
            }
        }
        
        last_part = part;
    }

    // Process remaining operations
    while (!operations.empty()) {
        Token last_operation = operations.top();
        operations.pop();
        
        if (last_operation == Token::OPEN_PARENTHESIS) {
            // CASE: TOO MANY OPEN-PARENTHESIS
            std::string message = "Unclosed '(': Too many open-parens in: " + text;
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

std::unique_ptr<Expression> parse(const std::string& text) {
    return TagExpressionParser::parse(text);
}

}  // namespace cucumber::tag_expressions
