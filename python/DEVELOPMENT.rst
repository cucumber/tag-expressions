Development of This Package
===============================================================================

Developer Workflow
-------------------------------------------------------------------------------

PROCEDURE:

* OPTIONAL STEP: Create an isolated virtual-environment for Python
* STEP: Install the Python packages with "pip" (in virtual-env or $HOME).
* STEP: Run the tests (ensure: ALL-TESTs are PASSING initially)
* Loop (until done):
  * Edit sources to extend the implementation or fix the implementation.
  * STEP: Run the tests (verify: that changes are OK)

* OPTIONAL STEP: Check test coverage
* OPTIONAL STEP: Check test with multiple Python versions by using `tox`_
  OTHERWISE: This is also check by the CI github-actions workflow "test-python".

PROCEDURE: Basics without a virtual-environment
-------------------------------------------------------------------------------

::

    $ cd ${THIS_DIR}
    $ pip install --extras -e .
    # -- HINT: Python packages are installed under the $HOME directory of the user.

    # -- STEP: Run the tests with "pytest" either in terse or verbose mode.
    # OUTPUTS: build/testing/report.html, build/testing/report.xml
    $ pytest
    $ pytest --verbose

    # -- STEP: Determine the test.coverage and generate test reports.
    # OUTPUT: build/coverage.html/index.html
    $ coverage run -m pytest
    $ coverage combine; coverage report; coverage html

USE CASE: Create a virtual-environment with "virtualenv" on UNIX
-------------------------------------------------------------------------------

:Covers: Linux, macOS, "Windows Subsystem for Linux" (WSL), ...

If virtualenv is not installed, install it (CASE: bash shell)::

    $ pip install virtualenv

Afterwards:

1. Create a virtual environment
2. Activate the virtual environment (case: bash or similar)
3. Install all required python packages

.. code-block:: bash

    $ virtualenv .venv
    $ source .venv/bin/activate
    $ pip install --extras -e .

    # -- HINT: Afterwards, to deactivate the virtual-environment, use:
    $ deactivate

SEE ALSO:

* https://virtualenv.pypa.io/en/latest/user_guide.html


USE CASE: Create a virtual-environment with "virtualenv" on Windows
-------------------------------------------------------------------------------

If virtualenv is not installed, install it by using the Windows cmd shell::

    cmd> pip install virtualenv

Afterwards:

1. Create a virtual environment in the cmd shell
2. Activate the virtual environment
3. Install all required python packages

.. code-block:: cmd

    cmd> virtualenv .venv
    cmd> call .venv/Scripts/activate
    cmd> pip install --extras -e .

SEE ALSO:

* https://virtualenv.pypa.io/en/latest/user_guide.html


USE CASE: Without virtual-environment
-------------------------------------------------------------------------------

Ensure that all required Python packages are installed::

    $ pip install --extras -e .

HINT: The Python packages are installed under the HOME directory of the user.


USE CASE: Run the Tests
-------------------------------------------------------------------------------

:PRECONDITION: Python packages are installed

`pytest`_ is used as test runner (and test framework) in Python.
Run the tests with::

    $ pytest          # Run tests in terse mode.
    $ pytest -v       # Run tests in verbose mode.
    $ pytest --html=report.html   # Run tests and create HTML test report.

Test for HTML (and JUnit XML) are generated at the end of the test run:

* ``build/testing/report.html``
* ``build/testing/report.xml``

SEE ALSO:

* https://pytest.org/
* https://pypi.org/project/pytest-html

.. _pytest: https://pytest.org/


USE CASE: Running the Tests with tox
-------------------------------------------------------------------------------

Tox allows to run tests against different python versions in isolated
virtual environments, one for each version.

To run the tests, use::

    $ tox -e py310     # Run tests in a virtual environment with python3.10

SEE ALSO:

* https://tox.wiki/
* https://pypi.org/project/tox
