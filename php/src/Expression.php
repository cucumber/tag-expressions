<?php

namespace Cucumber\TagExpressions;

interface Expression extends \Stringable
{
    /**
     * @param list<string> $variables
     */
    public function evaluate(array $variables): bool;
}
