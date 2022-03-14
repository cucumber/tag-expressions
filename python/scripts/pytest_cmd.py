#!/usr/bin/env python
# -- FOR PYTHON 2,7: Run pytest as Python module
# REASON: "pytest" seems no longer to be installed in "bin/" directory.

from __future__ import absolute_import, print_function
import sys
import pytest

if __name__ == "__main__":
    sys.exit(pytest.main())
