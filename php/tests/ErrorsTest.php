<?php

namespace Cucumber\TagExpressions\Tests;

use Cucumber\TagExpressions\TagExpressionException;
use Cucumber\TagExpressions\TagExpressionParser;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\TestCase;
use Symfony\Component\Yaml\Yaml;

class ErrorsTest extends TestCase
{
    #[DataProvider('provideExpectations')]
    public function testErrors(string $expression, string $error): void
    {
        $this->expectException(TagExpressionException::class);
        $this->expectExceptionMessage($error);

        TagExpressionParser::parse($expression);
    }

    /**
     * @return iterable<array{string, string}>
     */
    public static function provideExpectations(): iterable
    {
        /** @var list<array{expression: string, error: string}> $data */
        $data = Yaml::parseFile(__DIR__ . '/../../testdata/errors.yml');

        foreach ($data as $item) {
            yield [$item['expression'], $item['error']];
        }
    }
}
