# -*- coding: UTF-8 -*-
from __future__ import absolute_import, print_function
from collections import namedtuple
from pathlib import Path

from cucumber_tag_expressions.parser import TagExpressionParser
import pytest
import yaml


# -----------------------------------------------------------------------------
# DATA-FILE CONSTANTS:
# -----------------------------------------------------------------------------
HERE = Path(__file__).parent.absolute()
TESTDATA_DIRECTORY = HERE/"../../../testdata"
TESTDATA_FILE = TESTDATA_DIRECTORY/"parsing.yml"


# -----------------------------------------------------------------------------
# DATA-FILE DRIVEN TEST SUPPORT:
# -----------------------------------------------------------------------------
DTestData4Parsing = namedtuple("DTestData4Parsing", ("expression", "formatted"))

def read_testdata(data_filename):
    testdata_items = []
    with open(str(data_filename)) as f:
        for item in yaml.safe_load(f):
            assert isinstance(item, dict)
            data_item = DTestData4Parsing(item["expression"], item["formatted"])
            testdata_items.append(data_item)
    return testdata_items



# -----------------------------------------------------------------------------
# TEST SUITE:
# -----------------------------------------------------------------------------
this_testdata = read_testdata(TESTDATA_FILE)

@pytest.mark.parametrize("expression, formatted", this_testdata)
def test_parsing_with_datafile(expression, formatted):
    tag_expression = TagExpressionParser().parse(expression)
    actual_text = str(tag_expression)
    assert actual_text == formatted
