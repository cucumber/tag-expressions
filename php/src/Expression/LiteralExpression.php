<?php

namespace Cucumber\TagExpressions\Expression;

use Cucumber\TagExpressions\Expression;

/**
 * @internal
 */
final class LiteralExpression implements Expression
{
    public function __construct(
        private readonly string $value,
    ) {
    }

    public function evaluate(array $variables): bool
    {
        return \in_array($this->value, $variables, true);
    }

    public function __toString(): string
    {
        $value = str_replace(['\\', '(', ')'], ['\\\\', '\\(', '\\)'], $this->value);

        return preg_replace('/\s/', '\\\\ ', $value) ?? $value;
    }
}
