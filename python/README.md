<h1 align="center">
  <img src="https://raw.githubusercontent.com/cucumber/cucumber-js/46a5a78107be27e99c6e044c69b6e8f885ce456c/docs/images/logo.svg" alt="Cucumber logo" width="75">
  <br>
  Cucumber Tag Expressions
</h1>
<p align="center">
  <b>Readable boolean expressions to select features and scenarios marked with tags in Gherkin</b>
</p>

<p align="center">
  <a href="https://pypi.python.org/pypi/cucumber-tag-expressions">
    <img src="https://img.shields.io/pypi/v/cucumber-tag-expressions.svg?color=dark-green" alt="npm">
  </a>
  <a href="https://github.com/cucumber/tag-expressions/actions/workflows/test-python.yml">
    <img src="https://github.com/cucumber/tag-expressions/actions/workflows/test-python.yml/badge.svg" alt="build">
  </a>
  <a href="https://opencollective.com/cucumber">
    <img src="https://opencollective.com/cucumber/backers/badge.svg" alt="backers">
  </a>
  <a href="https://opencollective.com/cucumber">
    <img src="https://opencollective.com/cucumber/sponsors/badge.svg" alt="sponsors">
  </a>
  <a href="https://cucumber.io/docs/cucumber/api/#tag-expressions">
    <img src="https://img.shields.io/badge/-docs-brightgreen?logo=cucumber&logoColor=bright%20green&labelColor=grey" alt="build">
  </a>
</p>

```gherkin
# -- SIMPLE TAG-EXPRESSION EXAMPLES:
@a and @b
@a or @b
not @a

# -- MORE TAG-EXPRESSION EXAMPLES:
@a and not @b
(@a or @b) and not @c
```

## Getting Started

Cucumber Tag Expressions is available as [`cucumber-tag-expressions`](https://pypi.org/project/cucumber-tag-expressions/) on PyPI.

```console
pip install cucumber-tag-expressions
```

Parse tag expressions and evaluate them against a set of tags.

```python
>>> from cucumber_tag_expressions import parse

# Tagged with @fast
>>> fast = parse("@fast")
>>> fast({"@fast", "@wip"})
True
>>> fast({"@performance", "@slow"})
False

# Tagged with @wip and not @slow
>>> wip_not_slow = parse("@wip and not @slow")
>>> wip_not_slow({"@wip", "@home"})
True
>>> wip_not_slow({"wet", "warm", "raining"})
False

# Tagged with both `@fast` and `@integration`
>>> fast_integration = parse("@integration and @fast")
>>> fast_integration({"@integration", "@fast", "@other"})
True
>>> fast_integration({"@system", "@fast"})
False

# Tagged with either @login or @registration
>>> auth_pages = parse("@login or @registration")
>>> auth_pages({"@account", "@login"})
True
>>> auth_pages({"@admin", "@account"})
False

```

## Test Runner Usage

A gherkin test runner (such as [behave](https://behave.readthedocs.io/en/latest/)) uses tag expressions to select scenarios to run.

```shell
# -- TAG-EXPRESSION: @one and @two
# EXPECTED: Selects and runs scenario "Three".
$ my_cucumber_test_runner --tags="@one and @two" features/example.feature
...

# -- TAG-EXPRESSION: @one or @two
# EXPECTED: Selects and runs scenarios "One", "Two" and "Three".
$ my_cucumber_test_runner --tags="@one or @two" features/example.feature
...
```

Such as using the following feature file.

```gherkin
# -- FILE: features/example.feature
Feature: Tag expressions example

  @one
  Scenario: One
    Given a step passes

  @two
  Scenario: Two
    Given another step passes

  @one @two
  Scenario: Three
    Given some step passes

  Scenario: Four
    Given another step passes
```
