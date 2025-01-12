Cucumber Tag Expressions for Python
===============================================================================

.. |badge.CI_status| image:: https://github.com/cucumber/tag-expressions/actions/workflows/test-python.yml/badge.svg
    :target: https://github.com/cucumber/tag-expressions/actions/workflows/test-python.yml
    :alt: CI Build Status

.. |badge.latest_version| image:: https://img.shields.io/pypi/v/cucumber-tag-expressions.svg
    :target: https://pypi.python.org/pypi/cucumber-tag-expressions
    :alt: Latest Version

.. |badge.license| image:: https://img.shields.io/pypi/l/cucumber-tag-expressions.svg
    :target: https://pypi.python.org/pypi/cucumber-tag-expressions
    :alt: License

.. |badge.downloads| image:: https://img.shields.io/pypi/dm/cucumber-tag-expressions.svg
    :target: https://pypi.python.org/pypi/cucumber-tag-expressions
    :alt: Downloads

.. |logo| image:: https://github.com/cucumber-ltd/brand/raw/master/images/png/notm/cucumber-black/cucumber-black-128.png


|badge.CI_status| |badge.latest_version| |badge.license| |badge.downloads|

Cucumber tag-expressions for Python.

|logo|

Cucumber tag-expressions provide readable boolean expressions
to select features and scenarios marked with tags in Gherkin files
in an easy way::

    # -- SIMPLE TAG-EXPRESSION EXAMPLES:
    @a and @b
    @a or  @b
    not @a

    # -- MORE TAG-EXPRESSION EXAMPLES:
    @a and not @b
    (@a or @b) and not @c

SEE ALSO:

* https://cucumber.io/docs/cucumber/api/#tag-expressions

Getting Started
-----------------------------------------------------------------

Cucumber Tag Expressions is available as `cucumber-tag-expressions <https://pypi.org/project/cucumber-tag-expressions/>`_ on PyPI.

.. code-block:: console

    pip install cucumber-tag-expressions

Parse tag expressions and evaluate them against a set of tags.

.. code-block:: python

    >>> from cucumber_tag_expressions import parse
    >>> # Tagged with @fast
    >>> fast = parse("@fast")
    >>> fast({"@fast", "@wip"})
    True
    >>> fast({"@performance", "@slow"})
    False
    >>> # Tagged with @wip and not @slow
    >>> wip_not_slow = parse("@wip and not @slow")
    >>> wip_not_slow({"@wip", "@home"})
    True
    >>> wip_not_slow({"wet", "warm", "raining"})
    False
    >>> # Tagged with both `@fast` and `@integration`
    >>> fast_integration = parse("@integration and @fast")
    >>> fast_integration({"@integration", "@fast", "@other"})
    True
    >>> fast_integration({"@system", "@fast"})
    False
    >>> # Tagged with either @login or @registration
    >>> auth_pages = parse("@login or @registration")
    >>> auth_pages({"@account", "@login"})
    True
    >>> auth_pages({"@admin", "@account"})
    False

Test Runner Usage
-----------------------------------------------------------------

A cucumber test runner selects some scenarios by using tag-expressions and runs them:

.. code:: sh

    # -- TAG-EXPRESSION: @one and @two
    # EXPECTED: Selects and runs scenario "Three".
    $ my_cucumber_test_runner --tags="@one and @two" features/example.feature
    ...

    # -- TAG-EXPRESSION: @one or @two
    # EXPECTED: Selects and runs scenarios "One", "Two" and "Three".
    $ my_cucumber_test_runner --tags="@one or @two" features/example.feature
    ...

by using the following feature file:

.. code:: gherkin

    # -- FILE: features/example.feature
    Feature: Tag-Expressions Example

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
