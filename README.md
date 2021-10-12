[![test-go](https://github.com/cucumber/tag-expressions/actions/workflows/test-go.yml/badge.svg)](https://github.com/cucumber/tag-expressions/actions/workflows/test-go.yml)
[![test-java](https://github.com/cucumber/tag-expressions/actions/workflows/test-java.yml/badge.svg)](https://github.com/cucumber/tag-expressions/actions/workflows/test-java.yml)
[![test-javascript](https://github.com/cucumber/tag-expressions/actions/workflows/test-javascript.yml/badge.svg)](https://github.com/cucumber/tag-expressions/actions/workflows/test-javascript.yml)
[![test-perl](https://github.com/cucumber/tag-expressions/actions/workflows/test-perl.yml/badge.svg)](https://github.com/cucumber/tag-expressions/actions/workflows/test-perl.yml)
[![test-python](https://github.com/cucumber/tag-expressions/actions/workflows/test-python.yml/badge.svg)](https://github.com/cucumber/tag-expressions/actions/workflows/test-python.yml)
[![test-ruby](https://github.com/cucumber/tag-expressions/actions/workflows/test-ruby.yml/badge.svg)](https://github.com/cucumber/tag-expressions/actions/workflows/test-ruby.yml)

# Tag Expressions

Tag Expressions is a simple query language for tags. The simplest tag expression is
simply a single tag, for example:

    @smoke

A slightly more elaborate expression may combine tags, for example:

    @smoke and not @ui

Tag Expressions are used for two purposes:

1. Run a subset of scenarios (using the `--tags expression` option of the [command line](https://cucumber.io/docs/cucumber/api/#running-cucumber))
2. Specify that a hook should only run for a subset of scenarios (using [conditional hooks](https://cucumber.io/docs/cucumber/api/#hooks))

Tag Expressions are [boolean expressions](https://en.wikipedia.org/wiki/Boolean_expression)
of tags with the logical operators `and`, `or` and `not`.

For more complex Tag Expressions you can use parenthesis for clarity, or to change operator precedence:

    (@smoke or @ui) and (not @slow)

## Migrating from old style tags

Older versions of Cucumber used a different syntax for tags. The list below
provides some examples illustrating how to migrate to tag expressions.

* `--tags @dev` stays the same
* `--tags ~@dev` becomes `--tags 'not @dev'`
* `--tags @foo,@bar` becomes  `--tags '@foo or @bar'`
* `--tags @foo --tags @bar` becomes `--tags '@foo and bar'`
* `--tags ~@foo --tags @bar,@zap` becomes `--tags 'not @foo and (@bar or @zap)'`
