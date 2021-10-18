package io.cucumber.tagexpressions;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.MethodSource;
import org.yaml.snakeyaml.Yaml;

import java.io.IOException;
import java.nio.file.Paths;
import java.util.List;
import java.util.Map;

import static java.nio.file.Files.newInputStream;
import static org.junit.jupiter.api.Assertions.assertEquals;

class ParsingTest {

    private static List<Map<String, String>> acceptance_tests_pass() throws IOException {
        return new Yaml().loadAs(newInputStream(Paths.get("..", "testdata", "parsing.yml")), List.class);
    }

    @ParameterizedTest
    @MethodSource
    void acceptance_tests_pass(Map<String, String> expectation) {
        Expression expr = TagExpressionParser.parse(expectation.get("expression"));
        String formatted = expectation.get("formatted");
        assertEquals(formatted, expr.toString());

        Expression expr2 = TagExpressionParser.parse(formatted);
        assertEquals(formatted, expr2.toString());
    }
}
