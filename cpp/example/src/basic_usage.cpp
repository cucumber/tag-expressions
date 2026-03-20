#include <iostream>
#include <string>
#include <vector>

#include "cucumber/tag-expressions/parser.hpp"

int main() {
    using namespace cucumber::tag_expressions;

    std::cout << "Cucumber Tag Expressions - C++ Examples\n";
    std::cout << "========================================\n\n";

    // Example 1: Tagged with @fast
    std::cout << "Example 1: Tagged with @fast\n";
    auto fast = parse("@fast");
    std::cout << "  Expression: " << fast->to_string() << "\n";
    std::cout << "  {@fast, @wip}: " << (fast->evaluate({"@fast", "@wip"}) ? "true" : "false") << "\n";
    std::cout << "  {@Fast}: " << (fast->evaluate({"@Fast"}) ? "true" : "false") << "\n";
    std::cout << "  {@performance, @slow}: " << (fast->evaluate({"@performance", "@slow"}) ? "true" : "false") << "\n\n";

    // Example 2: Tagged with @wip and not @slow
    std::cout << "Example 2: Tagged with @wip and not @slow\n";
    auto wip_not_slow = parse("@wip and not @slow");
    std::cout << "  Expression: " << wip_not_slow->to_string() << "\n";
    std::cout << "  {@wip, @home}: " << (wip_not_slow->evaluate({"@wip", "@home"}) ? "true" : "false") << "\n";
    std::cout << "  {@wip, @slow}: " << (wip_not_slow->evaluate({"@wip", "@slow"}) ? "true" : "false") << "\n";
    std::cout << "  {wet, warm, raining}: " << (wip_not_slow->evaluate({"wet", "warm", "raining"}) ? "true" : "false") << "\n\n";

    // Example 3: Tagged with both @fast and @integration
    std::cout << "Example 3: Tagged with both @fast and @integration\n";
    auto fast_integration = parse("@integration and @fast");
    std::cout << "  Expression: " << fast_integration->to_string() << "\n";
    std::cout << "  {@integration, @fast, @other}: " 
              << (fast_integration->evaluate({"@integration", "@fast", "@other"}) ? "true" : "false") << "\n";
    std::cout << "  {@system, @fast}: " 
              << (fast_integration->evaluate({"@system", "@fast"}) ? "true" : "false") << "\n\n";

    // Example 4: Tagged with either @login or @registration
    std::cout << "Example 4: Tagged with either @login or @registration\n";
    auto auth_pages = parse("@login or @registration");
    std::cout << "  Expression: " << auth_pages->to_string() << "\n";
    std::cout << "  {@account, @login}: " 
              << (auth_pages->evaluate({"@account", "@login"}) ? "true" : "false") << "\n";
    std::cout << "  {@admin, @account}: " 
              << (auth_pages->evaluate({"@admin", "@account"}) ? "true" : "false") << "\n\n";

    // Example 5: Complex expression
    std::cout << "Example 5: Complex expression with parentheses\n";
    auto complex = parse("(@fast or @slow) and not @broken");
    std::cout << "  Expression: " << complex->to_string() << "\n";
    std::cout << "  {@fast, @passing}: " 
              << (complex->evaluate({"@fast", "@passing"}) ? "true" : "false") << "\n";
    std::cout << "  {@slow, @broken}: " 
              << (complex->evaluate({"@slow", "@broken"}) ? "true" : "false") << "\n";
    std::cout << "  {@other}: " 
              << (complex->evaluate({"@other"}) ? "true" : "false") << "\n\n";

    // Example 6: Error handling
    std::cout << "Example 6: Error handling\n";
    try {
        auto invalid = parse("@foo and and @bar");
        std::cout << "  Should have thrown an error!\n";
    } catch (const TagExpressionError& e) {
        std::cout << "  Caught expected error: " << e.what() << "\n";
    }

    std::cout << "\nAll examples completed successfully!\n";
    return 0;
}
