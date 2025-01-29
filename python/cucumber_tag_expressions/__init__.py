# -*- coding: UTF-8 -*-
"""Python implementation of `Cucumber Tag Expressions`_.

Tag expressions are used in cucumber, behave and other BDD frameworks
to select features, scenarios, etc. in `Gherkin`_ files.
These selected items are normally included in a test run.

.. _Cucumber Tag Expressions:
    https://cucumber.io/docs/cucumber/api/#tag-expressions

.. _Gherkin:
    https://cucumber.io/docs/gherkin/reference/
"""

from __future__ import absolute_import
from .parser import parse, TagExpressionParser, TagExpressionError
