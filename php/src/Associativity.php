<?php

namespace Cucumber\TagExpressions;

/**
 * @internal
 */
enum Associativity
{
    case LEFT;
    case RIGHT;

    /**
     * @param 'and'|'or'|'not' $operator
     */
    public static function forOperator(string $operator): self
    {
        return match ($operator) {
            'and', 'or' => self::LEFT,
            'not' => self::RIGHT,
        };
    }
}
