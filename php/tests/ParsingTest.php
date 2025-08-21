<?php

namespace Cucumber\TagExpressions\Tests;

use Cucumber\TagExpressions\TagExpressionParser;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\TestCase;
use Symfony\Component\Yaml\Yaml;

class ParsingTest extends TestCase
{
    #[DataProvider('provideExpectations')]
    public function testParsing(string $expression, string $formatted): void
    {
        $expr1 = TagExpressionParser::parse($expression);
        self::assertEquals($formatted, (string) $expr1);

        $expr2 = TagExpressionParser::parse($formatted);
        self::assertEquals($formatted, (string) $expr2);
    }

    /**
     * @return iterable<array{string, string}>
     */
    public static function provideExpectations(): iterable
    {
        /** @var list<array{expression: string, formatted: string}> $data */
        $data = Yaml::parseFile(__DIR__ . '/../../testdata/parsing.yml');

        foreach ($data as $item) {
            yield [$item['expression'], $item['formatted']];
        }
    }
}
