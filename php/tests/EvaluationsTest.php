<?php

namespace Cucumber\TagExpressions\Tests;

use Cucumber\TagExpressions\TagExpressionParser;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\TestCase;
use Symfony\Component\Yaml\Yaml;

class EvaluationsTest extends TestCase
{
    /**
     * @param list<string> $variables
     */
    #[DataProvider('provideExpectations')]
    public function testParsing(string $expression, array $variables, bool $result): void
    {
        $expr = TagExpressionParser::parse($expression);

        self::assertSame($result, $expr->evaluate($variables));
    }

    /**
     * @return iterable<array{string, list<string>, bool}>
     */
    public static function provideExpectations(): iterable
    {
        /** @var list<array{expression: string, tests: list<array{variables: list<string>, result: bool}>}> $data */
        $data = Yaml::parseFile(__DIR__ . '/../../testdata/evaluations.yml');

        foreach ($data as $item) {
            foreach ($item['tests'] as $test) {
                yield [$item['expression'], $test['variables'], $test['result']];
            }
        }
    }
}
