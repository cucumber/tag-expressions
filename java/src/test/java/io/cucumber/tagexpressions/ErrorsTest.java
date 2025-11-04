package io.cucumber.tagexpressions;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.MethodSource;
import org.yaml.snakeyaml.Yaml;

import java.io.IOException;
import java.nio.file.Paths;
import java.util.List;
import java.util.Map;

import static java.nio.file.Files.newInputStream;
import static java.util.Objects.requireNonNull;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

class ErrorsTest {

    static List<Map<String, String>> acceptance_tests_pass() throws IOException {
        return new Yaml().loadAs(newInputStream(Paths.get("..", "testdata", "errors.yml")), List.class);
    }

    @ParameterizedTest
    @MethodSource
    void acceptance_tests_pass(Map<String, String> expectation) {
        String expression = requireNonNull(expectation.get("expression"));
        TagExpressionException e = assertThrows(TagExpressionException.class,
                () -> TagExpressionParser.parse(expression));

        assertEquals(expectation.get("error"), e.getMessage());
    }
}
