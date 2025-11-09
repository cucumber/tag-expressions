<?php

namespace Cucumber\TagExpressions\Expression;

use Cucumber\TagExpressions\Expression;

/**
 * @internal
 */
final class TrueExpression implements Expression
{
    public function evaluate(array $variables): bool
    {
        return true;
    }

    public function __toString(): string
    {
        return '';
    }
}
