#include "cucumber-tag-expressions/expression.hpp"

#include <algorithm>
#include <sstream>

namespace cucumber {
namespace tag_expressions {

namespace {

    // Escape special characters in tag names for string representation
    std::string escape_string(const std::string& str) {
        std::ostringstream oss;
        for (char c : str) {
            if (c == '\\') {
                oss << "\\\\";
            } else if (c == '(') {
                oss << "\\(";
            } else if (c == ')') {
                oss << "\\)";
            } else if (std::isspace(static_cast<unsigned char>(c))) {
                oss << '\\' << c;
            } else {
                oss << c;
            }
        }
        return oss.str();
    }

}  // namespace

std::string Literal::to_string() const {
    return escape_string(name_);
}

std::string And::to_string() const {
    if (terms_.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << "( ";
    for (size_t i = 0; i < terms_.size(); ++i) {
        if (i > 0) {
            oss << " and ";
        }
        oss << terms_[i]->to_string();
    }
    oss << " )";
    return oss.str();
}

std::string Or::to_string() const {
    if (terms_.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << "( ";
    for (size_t i = 0; i < terms_.size(); ++i) {
        if (i > 0) {
            oss << " or ";
        }
        oss << terms_[i]->to_string();
    }
    oss << " )";
    return oss.str();
}

std::string Not::to_string() const {
    // Binary operators have parenthesis already
    if (dynamic_cast<And*>(term_.get()) || dynamic_cast<Or*>(term_.get())) {
        return "not " + term_->to_string();
    }
    return "not ( " + term_->to_string() + " )";
}

}  // namespace tag_expressions
}  // namespace cucumber
    