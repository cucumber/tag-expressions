<?php

namespace Cucumber\TagExpressions\Expression;

use Cucumber\TagExpressions\Expression;

/**
 * @internal
 */
final class NotExpression implements Expression
{
    public function __construct(
        private readonly Expression $expr,
    ) {
    }

    public function evaluate(array $variables): bool
    {
        return !$this->expr->evaluate($variables);
    }

    public function __toString(): string
    {
        if ($this->expr instanceof AndExpression || $this->expr instanceof OrExpression) {
            // -- HINT: Binary Operators already have already ' ( ... ) '.
            return 'not ' . $this->expr;
        }

        return 'not ( ' . $this->expr . ' )';
    }
}
