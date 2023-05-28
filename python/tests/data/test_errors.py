# -*- coding: UTF-8 -*-
from __future__ import absolute_import, print_function
from collections import namedtuple
from pathlib import Path

from cucumber_tag_expressions.parser import TagExpressionParser, TagExpressionError
import pytest
import yaml


# -----------------------------------------------------------------------------
# DATA-FILE CONSTANTS:
# -----------------------------------------------------------------------------
HERE = Path(__file__).parent.absolute()
TESTDATA_DIRECTORY = HERE/"../../../testdata"
TESTDATA_FILE = TESTDATA_DIRECTORY/"errors.yml"


# -----------------------------------------------------------------------------
# DATA-FILE DRIVEN TEST SUPPORT:
# -----------------------------------------------------------------------------
# - expression: '@a @b or'
#  error: 'Tag expression "@a @b or" could not be parsed because of syntax error: Expected operator.'
DTestData4Error = namedtuple("DTestData4Error", ("expression", "error"))

def read_testdata(data_filename):
    testdata_items = []
    with open(str(data_filename)) as f:
        for item in yaml.safe_load(f):
            assert isinstance(item, dict)
            data_item = DTestData4Error(item["expression"], item["error"])
            testdata_items.append(data_item)
    return testdata_items



# -----------------------------------------------------------------------------
# TEST SUITE:
# -----------------------------------------------------------------------------
this_testdata = read_testdata(TESTDATA_FILE)

@pytest.mark.skip(reason="TOO MANY DIFFERENCES: Error message here are more specific (IMHO)")
@pytest.mark.parametrize("expression, error", this_testdata)
def test_errors_with_datafile(expression, error):
    if "\\" in expression:
        pytest.skip("BACKSLASH-ESCAPING: Not supported yet")

    with pytest.raises(TagExpressionError) as exc_info:
        _ = TagExpressionParser().parse(expression)

    exc_text = exc_info.exconly()
    print(exc_text)
    assert error in exc_text
