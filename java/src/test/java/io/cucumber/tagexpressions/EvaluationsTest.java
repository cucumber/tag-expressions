package io.cucumber.tagexpressions;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.MethodSource;
import org.yaml.snakeyaml.Yaml;

import java.io.IOException;
import java.nio.file.Paths;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

import static java.nio.file.Files.newInputStream;
import static org.junit.jupiter.api.Assertions.assertEquals;

class EvaluationsTest {
    static class Expectation {
        final String expression;
        final List<String> variables;
        final boolean result;

        public Expectation(String expression, List<String> variables, boolean result) {
            this.expression = expression;
            this.variables = variables;
            this.result = result;
        }
    }

    private static List<Expectation> acceptance_tests_pass() throws IOException {
        List<Map<String, ?>> evaluations = new Yaml().loadAs(newInputStream(Paths.get("..", "testdata", "evaluations.yml")), List.class);
        return evaluations.stream().flatMap(map -> {
            String expression = (String) map.get("expression");
            List<Map<String, ?>> tests = (List<Map<String, ?>>) map.get("tests");
            return tests.stream().map(test -> {
                List<String> variables = (List<String>) test.get("variables");
                boolean result = (boolean) test.get("result");
                return new Expectation(expression, variables, result);
            });
        }).collect(Collectors.toList());
    }

    @ParameterizedTest
    @MethodSource
    void acceptance_tests_pass(Expectation expectation) {
        Expression expr = TagExpressionParser.parse(expectation.expression);
        expr.evaluate(expectation.variables);
        assertEquals(expectation.result, expr.evaluate(expectation.variables));
    }
}
