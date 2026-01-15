#pragma once

#include <memory>
#include <set>
#include <string>
#include <vector>

namespace cucumber {
    namespace tag_expressions {

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
            virtual bool evaluate(const std::set<std::string>& values) const = 0;

            /**
             * @brief Call operator to make an expression object callable.
             * 
             * @param values Tag names to evaluate
             * @return true if expression is true
             * @return false otherwise
             */
            bool operator()(const std::set<std::string>& values) const {
                return evaluate(values);
            }

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
            explicit Literal(std::string name) : name_(std::move(name)) {}

            bool evaluate(const std::set<std::string>& values) const override {
                return values.find(name_) != values.end();
            }

            std::string to_string() const override;

            const std::string& name() const { return name_; }

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
             * @param terms List of boolean expressions to AND
             */
            explicit And(std::vector<std::shared_ptr<Expression>> terms)
                : terms_(std::move(terms)) {}

            bool evaluate(const std::set<std::string>& values) const override {
                for (const auto& term : terms_) {
                    if (!term->evaluate(values)) {
                        return false;  // SHORTCUT: Any false makes the expression false
                    }
                }
                return true;  // OTHERWISE: All terms are true
            }

            std::string to_string() const override;

            const std::vector<std::shared_ptr<Expression>>& terms() const { return terms_; }

        private:
            std::vector<std::shared_ptr<Expression>> terms_;
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
             * @param terms List of boolean expressions to OR
             */
            explicit Or(std::vector<std::shared_ptr<Expression>> terms)
                : terms_(std::move(terms)) {}

            bool evaluate(const std::set<std::string>& values) const override {
                for (const auto& term : terms_) {
                    if (term->evaluate(values)) {
                        return true;  // SHORTCUT: Any true makes the expression true
                    }
                }
                return false;  // OTHERWISE: All terms are false
            }

            std::string to_string() const override;

            const std::vector<std::shared_ptr<Expression>>& terms() const { return terms_; }

        private:
            std::vector<std::shared_ptr<Expression>> terms_;
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
            explicit Not(std::shared_ptr<Expression> term) : term_(std::move(term)) {}

            bool evaluate(const std::set<std::string>& values) const override {
                return !term_->evaluate(values);
            }

            std::string to_string() const override;

            const std::shared_ptr<Expression>& term() const { return term_; }

        private:
            std::shared_ptr<Expression> term_;
        };

        /**
         * @brief Boolean expression that is always true.
         */
        class True : public Expression {
        public:
            bool evaluate(const std::set<std::string>& values) const override {
                (void)values;  // Unused parameter
                return true;
            }

            std::string to_string() const override {
                return "";
            }
        };

    } // namespace tag_expressions

}  // namespace cucumber
