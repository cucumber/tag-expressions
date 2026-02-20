#ifndef CUCUMBER_TAG_EXPRESSIONS_EXPRESSION_HPP_
#define CUCUMBER_TAG_EXPRESSIONS_EXPRESSION_HPP_

#include <memory>
#include <string>
#include <vector>

namespace cucumber::tag_expressions {

    /**
     * @brief Abstract base class for boolean expression terms of a tag expression
     * (or representing a parsed tag expression evaluation tree).
     */
    class Expression {
    public:
        virtual ~Expression() = default;

        /**
         * @brief Evaluate whether expression matches values.
         * 
         * @param values Tag names to evaluate
         * @return true if expression evaluates to true with values
         * @return false otherwise
         */
        virtual bool evaluate(const std::vector<std::string>& values) const = 0;

        /**
         * @brief Call operator to make an expression object callable.
         * 
         * @param values Tag names to evaluate
         * @return true if expression is true
         * @return false otherwise
         */
        bool operator()(const std::vector<std::string>& values) const;
        /**
         * @brief Convert expression to string representation.
         * 
         * @return std::string String representation of the expression
         */
        virtual std::string to_string() const = 0;
    };

    /**
     * @brief Used as placeholder for a tag in a boolean tag expression.
     */
    class Literal : public Expression {
    public:
        /**
         * @brief Construct a new Literal object.
         * 
         * @param name Tag name to represent as a literal
         */
        explicit Literal(std::string name);

        bool evaluate(const std::vector<std::string>& values) const override;

        std::string to_string() const override;

        /**
         * @brief Get the name of the literal.
         * 
         * @return std::string_view Name of the literal
         */
        std::string_view name() const;

    private:
        std::string name_;
    };

    /**
     * @brief Boolean-AND operation (as binary operation).
     * 
     * NOTE: Class supports more than two arguments.
     */
    class And : public Expression {
    public:
        /**
         * @brief Construct a Boolean-AND expression.
         * 
         * @param left Left boolean expression of the AND operation
         * @param right Right boolean expression of the AND operation
         */
        explicit And(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);

        bool evaluate(const std::vector<std::string>& values) const override;

        std::string to_string() const override;

        /**
         * @brief Get the left boolean expression of the AND operation.
         * 
         * @return const std::unique_ptr<Expression>& Left boolean expression
         */
        const std::unique_ptr<Expression>& left() const;

        /**
         * @brief Get the right boolean expression of the AND operation.
         * 
         * @return const std::unique_ptr<Expression>& Right boolean expression
         */
        const std::unique_ptr<Expression>& right() const;

    private:
        std::unique_ptr<Expression> left_;
        std::unique_ptr<Expression> right_;
    };

    /**
     * @brief Boolean-OR operation (as binary operation).
     * 
     * NOTE: Class supports more than two arguments.
     */
    class Or : public Expression {
    public:
        /**
         * @brief Construct a Boolean-OR expression.
         * 
         * @param left Left boolean expression of the OR operation
         * @param right Right boolean expression of the OR operation
         */
        explicit Or(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);

        bool evaluate(const std::vector<std::string>& values) const override;

        std::string to_string() const override;

        /**
         * @brief Get the left boolean expression of the OR operation.
         * 
         * @return const std::unique_ptr<Expression>& Left boolean expression
         */
        const std::unique_ptr<Expression>& left() const;

        /**
         * @brief Get the right boolean expression of the OR operation.
         * 
         * @return const std::unique_ptr<Expression>& Right boolean expression
         */
        const std::unique_ptr<Expression>& right() const;

    private:
        std::unique_ptr<Expression> left_;
        std::unique_ptr<Expression> right_;
    };

    /**
     * @brief Boolean-NOT operation (as unary operation).
     */
    class Not : public Expression {
    public:
        /**
         * @brief Construct a Boolean-NOT expression.
         * 
         * @param term Boolean expression to negate
         */
        explicit Not(std::unique_ptr<Expression> term);

        bool evaluate(const std::vector<std::string>& values) const override;

        std::string to_string() const override;

        /**
         * @brief Get the term in the NOT expression.
         * 
         * @return const std::unique_ptr<Expression>& Term being negated
         */
        const std::unique_ptr<Expression>& term() const;

    private:
        std::unique_ptr<Expression> term_;
    };

    /**
     * @brief Boolean expression that is always true.
     */
    class True : public Expression {
    public:
        bool evaluate([[maybe_unused]] const std::vector<std::string>& values) const override;

        std::string to_string() const override;
    };

}  // namespace cucumber::tag_expressions

#endif // CUCUMBER_TAG_EXPRESSIONS_EXPRESSION_HPP_
