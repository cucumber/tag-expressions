<?php

namespace Cucumber\TagExpressions\Expression;

use Cucumber\TagExpressions\Expression;

/**
 * @internal
 */
final class AndExpression implements Expression
{
    public function __construct(
        private readonly Expression $left,
        private readonly Expression $right,
    ) {
    }

    public function evaluate(array $variables): bool
    {
        return $this->left->evaluate($variables) && $this->right->evaluate($variables);
    }

    public function __toString(): string
    {
        return '( ' . $this->left . ' and ' . $this->right . ' )';
    }
}
