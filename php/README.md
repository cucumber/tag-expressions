# Cucumber Tag Expressions

This is a PHP implementation of the [Cucumber Tag Expressions parser](https://github.com/cucumber/tag-expressions)

## Requirements

* PHP 8.1

## Installation

Install using [composer](https://getcomposer.org).

```shell
composer require cucumber/tag-expressions
```

## Usage

```php
use Cucumber\TagExpressions\TagExpressionParser;

$expression = TagExpressionParser::parse('@smoke and not @ui');

$result = $expression->evaluate(['@smoke', '@cli'])
```
