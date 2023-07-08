package io.cucumber.tagexpressions;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Deque;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public final class TagExpressionParser {
    //regex for operands to ensure no oprerand has ',' in them later can be customized further
    private final static String VALID_OPERAND = "^[^,]*$";
    private static final Map<String, Assoc> ASSOC = new HashMap<String, Assoc>() {{
        put("or", Assoc.LEFT);
        put("and", Assoc.LEFT);
        put("not", Assoc.RIGHT);
    }};
    private static final Map<String, Integer> PREC = new HashMap<String, Integer>() {{
        put("(", -2);
        put(")", -1);
        put("or", 0);
        put("and", 1);
        put("not", 2);
    }};
    private static final char ESCAPING_CHAR = '\\';
    private final String infix;

    public static Expression parse(String infix) {
        return new TagExpressionParser(infix).parse();
    }

    private TagExpressionParser(String infix) {
        this.infix = infix;
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
                while (operators.size() > 0 && isOperator(operators.peek()) && (
                        (ASSOC.get(token) == Assoc.LEFT && PREC.get(token) <= PREC.get(operators.peek()))
                                ||
                                (ASSOC.get(token) == Assoc.RIGHT && PREC.get(token) < PREC.get(operators.peek())))
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
                while (operators.size() > 0 && !"(".equals(operators.peek())) {
                    pushExpr(pop(operators), expressions);
                }
                if (operators.size() == 0) {
                    throw new TagExpressionException("Tag expression \"%s\" could not be parsed because of syntax error: Unmatched ).", this.infix);
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

        while (operators.size() > 0) {
            if ("(".equals(operators.peek())) {
                throw new TagExpressionException("Tag expression \"%s\" could not be parsed because of syntax error: Unmatched (.", infix);
            }
            pushExpr(pop(operators), expressions);
        }

        return expressions.pop();
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
                    throw new TagExpressionException("Tag expression \"%s\" could not be parsed because of syntax error: Illegal escape before \"%s\".", expr, c);
                }
            } else if (c == ESCAPING_CHAR) {
                isEscaped = true;
            } else if (c == '(' || c == ')' || Character.isWhitespace(c)) {
                if (token.length() > 0) {
                    isOperandValid(token,expr);
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
        isOperandValid(token,expr);
        if (token.length() > 0)  {
            tokens.add(token.toString());
        }
        return tokens;
    }

    /**
     * this method checks if the operand comply with the req
     * regex if not throws exception
     * @param token supposed tag of token
     * @param expr entire expression
     */
    private static void isOperandValid(StringBuilder token,String expr){
        if(!String.valueOf(token).matches(VALID_OPERAND)){
            throw new TagExpressionException("Tag expression \"%s\" could not be parsed because of syntax error: Tag names should follow this pattern \"%s\"",
             expr, VALID_OPERAND);
        }

    }

    private void check(TokenType expectedTokenType, TokenType tokenType) {
        if (expectedTokenType != tokenType) {
            throw new TagExpressionException("Tag expression \"%s\" could not be parsed because of syntax error: Expected %s.", infix, expectedTokenType.toString().toLowerCase());
        }
    }

    private <T> T pop(Deque<T> stack) {
        if (stack.isEmpty())
            throw new TagExpressionException("Tag expression \"%s\" could not be parsed because of an empty stack", infix);
        return stack.pop();
    }

    private void pushExpr(String token, Deque<Expression> stack) {
        switch (token) {
            case "and":
                Expression rightAndExpr = pop(stack);
                stack.push(new And(pop(stack), rightAndExpr));
                break;
            case "or":
                Expression rightOrExpr = pop(stack);
                stack.push(new Or(pop(stack), rightOrExpr));
                break;
            case "not":
                stack.push(new Not(pop(stack)));
                break;
            default:
                stack.push(new Literal(token));
                break;
        }
    }

    private boolean isUnary(String token) {
        return "not".equals(token);
    }

    private boolean isBinary(String token) {
        return "or".equals(token) || "and".equals(token);
    }

    private boolean isOperator(String token) {
        return ASSOC.get(token) != null;
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
            return "( " + left.toString() + " or " + right.toString() + " )";
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
            return "( " + left.toString() + " and " + right.toString() + " )";
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
            if (And.class.isInstance(expr) || Or.class.isInstance(expr)) {
                // -- HINT: Binary Operators already have already ' ( ... ) '.
                return "not " + expr.toString();
            }
            return "not ( " + expr.toString() + " )";
        }
    }

    private static class True implements Expression {
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
