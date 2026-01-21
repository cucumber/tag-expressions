#include "cucumber/tag-expressions/expression.hpp"

#include <algorithm>
#include <sstream>

namespace cucumber::tag_expressions {

namespace {

    // Escape special characters in tag names for string representation
    std::string escape_string(const std::string& str) {
        std::ostringstream oss;
        for (char c : str) {
            if (c == '\\') {
                oss << R"(\\)";
            } else if (c == '(') {
                oss << R"(\()";
            } else if (c == ')') {
                oss << R"(\))";
            } else if (std::isspace(static_cast<unsigned char>(c))) {
                oss << '\\' << c;
            } else {
                oss << c;
            }
        }
        return oss.str();
    }

}  // namespace

bool Expression::operator()(const std::vector<std::string>& values) const {
    return evaluate(values);
}

Literal::Literal(std::string name) :
    name_{std::move(name)} {
}

bool Literal::evaluate(const std::vector<std::string>& values) const {
    return std::find(values.begin(), values.end(), name_) != values.end();
}

std::string Literal::to_string() const {
    return escape_string(name_);
}

std::string_view Literal::name() const {
    return name_;
}

And::And(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right) :
    left_{std::move(left)},
    right_{std::move(right)} {
}

bool And::evaluate(const std::vector<std::string>& values) const {
    return left_->evaluate(values) && right_->evaluate(values);
}

std::string And::to_string() const {
    std::ostringstream oss;
    oss << "( " << left_->to_string() << " and " << right_->to_string() << " )";
    return oss.str();
}

const std::unique_ptr<Expression>& And::left() const {
    return left_;
}

const std::unique_ptr<Expression>& And::right() const {
    return right_;
}

Or::Or(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right) :
    left_{std::move(left)},
    right_{std::move(right)} {
}

bool Or::evaluate(const std::vector<std::string>& values) const {
    return left_->evaluate(values) || right_->evaluate(values);
}

std::string Or::to_string() const {
    std::ostringstream oss;
    oss << "( " << left_->to_string() << " or " << right_->to_string() << " )";
    return oss.str();
}

const std::unique_ptr<Expression>& Or::left() const {
    return left_;
}

const std::unique_ptr<Expression>& Or::right() const {
    return right_;
}

Not::Not(std::unique_ptr<Expression> term) :
    term_{std::move(term)} {
}

bool Not::evaluate(const std::vector<std::string>& values) const {
    return !term_->evaluate(values);
}

std::string Not::to_string() const {
    // Binary operators have parenthesis already
    if (dynamic_cast<And*>(term_.get()) || dynamic_cast<Or*>(term_.get())) {
        return "not " + term_->to_string();
    }
    return "not ( " + term_->to_string() + " )";
}

const std::unique_ptr<Expression>& Not::term() const {
    return term_;
}

bool True::evaluate([[maybe_unused]] const std::vector<std::string>& values) const {
    return true;
}

std::string True::to_string() const {
    return "";
}

}  // namespace cucumber::tag_expressions
