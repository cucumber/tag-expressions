package io.cucumber.tagexpressions;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Deque;
import java.util.List;
import java.util.Locale;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static java.util.Objects.requireNonNull;

public final class TagExpressionParser {
    private static boolean assoc(String token, Assoc assoc) {
        return switch (token) {
            case "or", "and" -> assoc == Assoc.LEFT;
            case "not" -> assoc == Assoc.RIGHT;
            default -> throw new IllegalArgumentException(token);
        };
    }

    private static int prec(String token) {
        return switch (token) {
            case "(" -> -2;
            case ")" -> -1;
            case "or" -> 0;
            case "and" -> 1;
            case "not" -> 2;
            default -> throw new IllegalArgumentException(token);
        };
    }

    private static final char ESCAPING_CHAR = '\\';
    private final String infix;

    public static Expression parse(String infix) {
        return new TagExpressionParser(infix).parse();
    }

    private TagExpressionParser(String infix) {
        this.infix = requireNonNull(infix);
    }

    private Expression parse() {
        List<String> tokens = tokenize(infix);
        if (tokens.isEmpty()) return new True();

        Deque<String> operators = new ArrayDeque<>();
        Deque<Expression> expressions = new ArrayDeque<>();
        TokenType expectedTokenType = TokenType.OPERAND;
        for (String token : tokens) {
            if (isUnary(token)) {
                check(expectedTokenType, TokenType.OPERAND);
                operators.push(token);
                expectedTokenType = TokenType.OPERAND;
            } else if (isBinary(token)) {
                check(expectedTokenType, TokenType.OPERATOR);
                while (!operators.isEmpty() && isTokenForOperator(token, operators.peek())
                ) {
                    pushExpr(pop(operators), expressions);
                }
                operators.push(token);
                expectedTokenType = TokenType.OPERAND;
            } else if ("(".equals(token)) {
                check(expectedTokenType, TokenType.OPERAND);
                operators.push(token);
                expectedTokenType = TokenType.OPERAND;
            } else if (")".equals(token)) {
                check(expectedTokenType, TokenType.OPERATOR);
                while (!operators.isEmpty() && !"(".equals(operators.peek())) {
                    pushExpr(pop(operators), expressions);
                }
                if (operators.isEmpty()) {
                    throw new TagExpressionException("Tag expression \"%s\" could not be parsed because of syntax error: Unmatched ).".formatted(this.infix));
                }
                if ("(".equals(operators.peek())) {
                    pop(operators);
                }
                expectedTokenType = TokenType.OPERATOR;
            } else {
                check(expectedTokenType, TokenType.OPERAND);
                pushExpr(token, expressions);
                expectedTokenType = TokenType.OPERATOR;
            }
        }

        while (!operators.isEmpty()) {
            if ("(".equals(operators.peek())) {
                throw new TagExpressionException("Tag expression \"%s\" could not be parsed because of syntax error: Unmatched (.".formatted(infix));
            }
            pushExpr(pop(operators), expressions);
        }

        return expressions.pop();
    }

    @SuppressWarnings("UnnecessaryParentheses")
    private boolean isTokenForOperator(String token, String operator) {
        if (!isOperator(operator)) {
            return false;
        }
        return (assoc(token, Assoc.LEFT) && prec(token) <= prec(operator)) ||
                (assoc(token, Assoc.RIGHT) && prec(token) < prec(operator));
    }

    private static List<String> tokenize(String expr) {
        List<String> tokens = new ArrayList<>();
        boolean isEscaped = false;
        StringBuilder token = new StringBuilder();
        for (int i = 0; i < expr.length(); i++) {
            char c = expr.charAt(i);
            if (isEscaped) {
                if (c == '(' || c == ')' || c == '\\' || Character.isWhitespace(c)) {
                    token.append(c);
                    isEscaped = false;
                } else {
                    throw new TagExpressionException("Tag expression \"%s\" could not be parsed because of syntax error: Illegal escape before \"%s\".".formatted(expr, c));
                }
            } else if (c == ESCAPING_CHAR) {
                isEscaped = true;
            } else if (c == '(' || c == ')' || Character.isWhitespace(c)) {
                if (!token.isEmpty()) {
                    tokens.add(token.toString());
                    token = new StringBuilder();
                }
                if (!Character.isWhitespace(c)) {
                    tokens.add(String.valueOf(c));
                }
            } else {
                token.append(c);
            }
        }
        if (!token.isEmpty()) {
            tokens.add(token.toString());
        }
        return tokens;
    }

    private void check(TokenType expectedTokenType, TokenType tokenType) {
        if (expectedTokenType != tokenType) {
            throw new TagExpressionException("Tag expression \"%s\" could not be parsed because of syntax error: Expected %s.".formatted(infix, expectedTokenType.toString().toLowerCase(Locale.ROOT)));
        }
    }

    private <T> T pop(Deque<T> stack) {
        if (stack.isEmpty())
            throw new TagExpressionException("Tag expression \"%s\" could not be parsed because of an empty stack".formatted(infix));
        return stack.pop();
    }

    private void pushExpr(String token, Deque<Expression> stack) {
        Expression expression = switch (token) {
            case "and" -> {
                Expression rightAndExpr = pop(stack);
                Expression leftAndExpr = pop(stack);
                yield new And(leftAndExpr, rightAndExpr);
            }
            case "or" -> {
                Expression rightOrExpr = pop(stack);
                Expression leftOrExpr = pop(stack);
                yield new Or(leftOrExpr, rightOrExpr);
            }
            case "not" -> new Not(pop(stack));
            default -> new Literal(token);
        };
        stack.push(expression);
    }

    private boolean isUnary(String token) {
        return "not".equals(token);
    }

    private boolean isBinary(String token) {
        return "or".equals(token) || "and".equals(token);
    }

    private boolean isOperator(String token) {
        return isBinary(token) || isUnary(token);
    }

    private enum TokenType {
        OPERAND,
        OPERATOR
    }

    private enum Assoc {
        LEFT,
        RIGHT
    }

    private static class Literal implements Expression {
        private final String value;

        Literal(String value) {
            this.value = value;
        }

        @Override
        public boolean evaluate(List<String> variables) {
            return variables.contains(value);
        }

        @Override
        public String toString() {
            return value
                    .replaceAll(Pattern.quote("\\"), Matcher.quoteReplacement("\\\\"))
                    .replaceAll(Pattern.quote("("), Matcher.quoteReplacement("\\("))
                    .replaceAll(Pattern.quote(")"), Matcher.quoteReplacement("\\)"))
                    .replaceAll("\\s", "\\\\ ");
        }
    }

    private static class Or implements Expression {
        private final Expression left;
        private final Expression right;

        Or(Expression left, Expression right) {
            this.left = left;
            this.right = right;
        }

        @Override
        public boolean evaluate(List<String> variables) {
            return left.evaluate(variables) || right.evaluate(variables);
        }

        @Override
        public String toString() {
            return "( " + left + " or " + right + " )";
        }
    }

    private static class And implements Expression {
        private final Expression left;
        private final Expression right;

        And(Expression left, Expression right) {
            this.left = left;
            this.right = right;
        }

        @Override
        public boolean evaluate(List<String> variables) {
            return left.evaluate(variables) && right.evaluate(variables);
        }

        @Override
        public String toString() {
            return "( " + left + " and " + right + " )";
        }
    }

    private static class Not implements Expression {
        private final Expression expr;

        Not(Expression expr) {
            this.expr = expr;
        }

        @Override
        public boolean evaluate(List<String> variables) {
            return !expr.evaluate(variables);
        }

        @Override
        public String toString() {
            if (expr instanceof And || expr instanceof Or) {
                // -- HINT: Binary Operators already have already ' ( ... ) '.
                return "not " + expr;
            }
            return "not ( " + expr + " )";
        }
    }

    private static final class True implements Expression {
        @Override
        public boolean evaluate(List<String> variables) {
            return true;
        }

        @Override
        public String toString() {
            return "true";
        }
    }
}
