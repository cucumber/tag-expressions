#ifndef TAG_EXPRESSION_PARSER_HPP
#define TAG_EXPRESSION_PARSER_HPP

#include "cucumber/tag-expressions/Model.hpp"
#include <memory>
#include <string_view>

namespace cucumber::tag_expressions
{
    std::unique_ptr<Expression> Parse(std::string_view expression);
}

#endif
