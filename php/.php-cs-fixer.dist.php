<?php

use PhpCsFixer\Config;
use PhpCsFixer\Finder;
use PhpCsFixer\Runner\Parallel\ParallelConfig;

$finder = Finder::create()
    ->in(__DIR__ . '/src')
    ->in(__DIR__ . '/tests')
    ->append([__DIR__ . '/.php-cs-fixer.dist.php'])
;

$config = new Config();

return $config
    ->setParallelConfig(new ParallelConfig())
    ->setRiskyAllowed(true)
    ->setRules([
        '@Symfony' => true,
        'yoda_style' => false,
        'concat_space' => ['spacing' => 'one'],
    ])
    ->setFinder($finder)
    ->setCacheFile(__DIR__ . '/.php-cs-fixer.cache')
;
