<?php

namespace Cucumber\TagExpressions;

use Cucumber\TagExpressions\Expression\AndExpression;
use Cucumber\TagExpressions\Expression\LiteralExpression;
use Cucumber\TagExpressions\Expression\NotExpression;
use Cucumber\TagExpressions\Expression\OrExpression;
use Cucumber\TagExpressions\Expression\TrueExpression;

final class TagExpressionParser
{
    public static function parse(string $infix): Expression
    {
        return (new self($infix))->parseInfix();
    }

    private const ESCAPING_CHAR = '\\';
    private const PREC = [
        '(' => -2,
        ')' => -1,
        'or' => 0,
        'and' => 1,
        'not' => 2,
    ];

    private function __construct(
        private readonly string $infix,
    ) {
    }

    private function parseInfix(): Expression
    {
        $tokens = self::tokenize($this->infix);
        if (\count($tokens) === 0) {
            return new TrueExpression();
        }
        /** @var list<string> $operators */
        $operators = [];
        $expressions = [];
        $expectedTokenType = TokenType::Operand;

        foreach ($tokens as $token) {
            if ($token === 'not') {
                $this->check($expectedTokenType, TokenType::Operand);
                $operators[] = $token;
                $expectedTokenType = TokenType::Operand;
            } elseif ($token === 'and' || $token === 'or') {
                $this->check($expectedTokenType, TokenType::Operator);
                while (\count($operators) > 0 && self::isOperator(self::peek($operators)) && (
                    (Associativity::forOperator($token) === Associativity::LEFT && self::PREC[$token] <= self::PREC[self::peek($operators)])
                    || (Associativity::forOperator($token) === Associativity::RIGHT && self::PREC[$token] < self::PREC[self::peek($operators)])
                )) {
                    $this->pushExpr(array_pop($operators), $expressions);
                }
                // TODO check associativity
                $operators[] = $token;
                $expectedTokenType = TokenType::Operand;
            } elseif ($token === '(') {
                $this->check($expectedTokenType, TokenType::Operand);
                $operators[] = $token;
                $expectedTokenType = TokenType::Operand;
            } elseif ($token === ')') {
                $this->check($expectedTokenType, TokenType::Operator);
                while (\count($operators) > 0 && self::peek($operators) !== '(') {
                    $this->pushExpr(array_pop($operators), $expressions);
                }

                if (\count($operators) === 0) {
                    throw new TagExpressionException(\sprintf('Tag expression "%s" could not be parsed because of syntax error: Unmatched ).', $this->infix));
                }

                if (self::peek($operators) === '(') {
                    array_pop($operators);
                }

                $expectedTokenType = TokenType::Operator;
            } else {
                $this->check($expectedTokenType, TokenType::Operand);
                $this->pushExpr($token, $expressions);
                $expectedTokenType = TokenType::Operator;
            }
        }

        while (\count($operators) > 0) {
            if (self::peek($operators) === '(') {
                throw new TagExpressionException(\sprintf('Tag expression "%s" could not be parsed because of syntax error: Unmatched (.', $this->infix));
            }

            $this->pushExpr($this->popOperand($operators), $expressions);
        }

        return array_pop($expressions);
    }

    /**
     * @return list<string>
     */
    private static function tokenize(string $expr): array
    {
        $tokens = [];
        $isEscaped = false;
        $token = '';

        for ($i = 0; $i < \strlen($expr); ++$i) {
            $c = $expr[$i];
            if ($isEscaped) {
                if ($c === '(' || $c === ')' || $c === '\\' || preg_match('/\s/', $c)) {
                    $token .= $c;
                    $isEscaped = false;
                } else {
                    throw new TagExpressionException(\sprintf('Tag expression "%s" could not be parsed because of syntax error: Illegal escape before "%s".', $expr, $c));
                }
            } elseif ($c === self::ESCAPING_CHAR) {
                $isEscaped = true;
            } elseif ($c === '(' || $c === ')' || preg_match('/\s/', $c)) {
                if ($token !== '') {
                    $tokens[] = $token;
                    $token = '';
                }

                if ($c === '(' || $c === ')') {
                    $tokens[] = $c;
                }
            } else {
                $token .= $c;
            }
        }

        if ($token !== '') {
            $tokens[] = $token;
        }

        return $tokens;
    }

    private function check(TokenType $expectedTokenType, TokenType $tokenType): void
    {
        if ($expectedTokenType !== $tokenType) {
            throw new TagExpressionException(\sprintf('Tag expression "%s" could not be parsed because of syntax error: Expected %s.', $this->infix, strtolower($expectedTokenType->name)));
        }
    }

    /**
     * @param non-empty-list<string> $stack
     */
    private static function peek(array $stack): string
    {
        return $stack[\count($stack) - 1];
    }

    /**
     * @template T
     *
     * @param list<T> $stack
     *
     * @return T
     */
    private function popOperand(array &$stack): mixed
    {
        $value = array_pop($stack);

        if ($value === null) {
            throw new TagExpressionException(\sprintf('Tag expression "%s" could not be parsed because of syntax error: Expression is incomplete.', $this->infix));
        }

        return $value;
    }

    /**
     * @param list<Expression> $stack
     */
    private function pushExpr(string $token, array &$stack): void
    {
        switch ($token) {
            case 'and':
                $rightAndExpr = $this->popOperand($stack);
                $lefAndExpr = $this->popOperand($stack);
                $stack[] = new AndExpression($lefAndExpr, $rightAndExpr);
                break;

            case 'or':
                $rightOrExpr = $this->popOperand($stack);
                $leftOrExpr = $this->popOperand($stack);
                $stack[] = new OrExpression($leftOrExpr, $rightOrExpr);
                break;

            case 'not':
                $stack[] = new NotExpression($this->popOperand($stack));
                break;

            default:
                $stack[] = new LiteralExpression($token);
        }
    }

    /**
     * @phpstan-assert-if-true 'and'|'or'|'not' $token
     */
    private static function isOperator(string $token): bool
    {
        return 'and' === $token || 'or' === $token || 'not' === $token;
    }
}
