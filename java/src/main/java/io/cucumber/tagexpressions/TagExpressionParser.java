package io.cucumber.tagexpressions;

import java.util.*;

public class TagExpressionParser {
    private enum Assoc {
        LEFT,
        RIGHT
    }

    private static Map<String, Assoc> ASSOC = new HashMap<String, Assoc>() {{
        put("or", Assoc.LEFT);
        put("and", Assoc.LEFT);
        put("not", Assoc.RIGHT);
    }};

    private static Map<String, Integer> PREC = new HashMap<String, Integer>() {{
        put("(", -2);
        put(")", -1);
        put("or", 0);
        put("and", 1);
        put("not", 2);
    }};

    private static char ESCAPING_CHAR = '\\';


    private static String[] tokenize(String expr) {
        List<String> tokens = new ArrayList<String>();

        boolean isEscaped = false;
        StringBuilder token = null;
        for (int i = 0; i < expr.length(); i++) {
            char c = expr.charAt(i);
            if (ESCAPING_CHAR == c) {
                isEscaped = true;
            } else {
                if (Character.isWhitespace(c)) { // skip
                    if (null != token) { // end of token
                        tokens.add(token.toString());
                        token = null;
                    }
                } else {
                    switch (c) {
                        case '(':
                        case ')':
                            if (!isEscaped) {
                                if (null != token) { // end of token
                                    tokens.add(token.toString());
                                    token = null;
                                }
                                tokens.add(String.valueOf(c));
                                break;
                            }
                        default:
                            if (null == token) { // start of token
                                token = new StringBuilder();
                            }
                            token.append(c);
                            break;
                    }
                }
                isEscaped = false;
            }
        }
        if (null != token) { // end of token
            tokens.add(token.toString());
        }
        return tokens.toArray(new String[0]);
    }

    public Expression parse(String infix) {
        String[] tokens = tokenize(infix);
        Deque<String> ops = new ArrayDeque<>();
        Deque<Expression> exprs = new ArrayDeque<>();

        for (String token : tokens) {
            if (isOp(token)) {
                while (ops.size() > 0 && isOp(ops.peek()) && (
                        (ASSOC.get(token) == Assoc.LEFT && PREC.get(token) <= PREC.get(ops.peek()))
                                ||
                                (ASSOC.get(token) == Assoc.RIGHT && PREC.get(token) < PREC.get(ops.peek())))
                        ) {
                    pushExpr(pop(ops), exprs);
                }
                ops.push(token);
            } else if ("(".equals(token)) {
                ops.push(token);
            } else if (")".equals(token)) {
                while (ops.size() > 0 && !"(".equals(ops.peek())) {
                    pushExpr(pop(ops), exprs);
                }
                if (ops.size() == 0) {
                    throw new RuntimeException("Unclosed (");
                }
                if ("(".equals(ops.peek())) {
                    pop(ops);
                }
            } else {
                pushExpr(token, exprs);
            }
        }

        while (ops.size() > 0) {
            if ("(".equals(ops.peek())) {
                throw new Error("Unclosed (");
            }
            pushExpr(pop(ops), exprs);
        }

        Expression expr = exprs.pop();
        if (exprs.size() > 0) {
            throw new Error("Not empty");
        }
        return expr;
    }


    private <T> T pop(Deque<T> stack) {
        if (stack.isEmpty()) throw new TagExpressionException("empty stack");
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

    private boolean isOp(String token) {
        return ASSOC.get(token) != null;
    }

    private class Literal implements Expression {
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
            return value.replaceAll("\\(", "\\\\(").replaceAll("\\)", "\\\\)");
        }
    }

    private class Or implements Expression {
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

    private class And implements Expression {
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

    private class Not implements Expression {
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
            return "not ( " + expr.toString() + " )";
        }
    }
}
