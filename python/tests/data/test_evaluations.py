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
TESTDATA_FILE = TESTDATA_DIRECTORY/"evaluations.yml"


# -----------------------------------------------------------------------------
# DATA-FILE DRIVEN TEST SUPPORT:
# -----------------------------------------------------------------------------
# - expression: 'not x'
#   tests:
#     - variables: ['x']
#       result: false
#     - variables: ['y']
#       result: true
DTestData4Evaluation = namedtuple("DTestData4Evaluation", ("expression", "tests"))
DTestVarsAndResult = namedtuple("DTestVarsAndResult", ("variables", "result"))

def read_testdata(data_filename):
    testdata_items = []
    with open(str(data_filename)) as f:
        for item in yaml.safe_load(f):
            assert isinstance(item, dict)
            tests = []
            for test in item["tests"]:
                test_variables = test["variables"]
                test_result = test["result"]
                tests.append(DTestVarsAndResult(test_variables, test_result))
            data_item = DTestData4Evaluation(item["expression"], tests)
            testdata_items.append(data_item)
    return testdata_items



# -----------------------------------------------------------------------------
# TEST SUITE:
# -----------------------------------------------------------------------------
this_testdata = read_testdata(TESTDATA_FILE)

@pytest.mark.parametrize("expression, tests", this_testdata)
def test_parsing_with_datafile(expression, tests):
    print("expression := {0}".format(expression))
    tag_expression = TagExpressionParser().parse(expression)
    for test_data in tests:
        print("test.variables= {0}".format(test_data.variables))
        print("test.result   = {0}".format(test_data.result))
        actual_result = tag_expression.evaluate(test_data.variables)
        assert actual_result == test_data.result
