Development of This Package
===============================================================================

ASSUMPTIONS:

* Python >= 2.7 or Python >= 3.6 is installed
* pip is installed (should be bundled with python nowadays)

Check if ``python`` and ``pip`` is installed::

    python --version
    pip --version


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
    $ pip install -r py.requirements/all.txt
    # -- HINT: Python packages are installed under the $HOME directory of the user.

    # -- STEP: Run the tests with "pytest" either in terse or verbose mode.
    # OUTPUTS: build/testing/report.html, build/testing/report.xml
    $ pytest
    $ pytest --verbose

    # -- STEP: Determine the test.coverage and generate test reports.
    # OUTPUT: build/coverage.html/index.html
    $ coverage run -m pytest
    $ coverage combine; coverage report; coverage html



PROCEDURE: By using "make" (on UNIX platforms, like: Linux, macOS, ...)
-------------------------------------------------------------------------------

.. code-block:: bash

    # -- HINTS:
    # The Make default-target:
    #   * Ensures that all packages are installed
    #   * Runs the tests
    $ make

    # -- STEP: Install/Update all Python packages (explicitely).
    # ALTERNATIVE: make update-dependencies
    $ make install-packages

    # -- STEP: Run the tests with "pytest" either in terse or verbose mode.
    # OUTPUTS: build/testing/report.html, build/testing/report.xml
    $ make test
    $ make test PYTEST_ARGS="--verbose"

    # -- STEP: Determine the test.coverage and generate test reports.
    # OUTPUT: build/coverage.html/index.html
    $ make test.coverage

    # -- OPTIONAL: Cleanup afterwards
    $ make clean


PROCEDURE: By using "invoke" build system (on all platforms)
-------------------------------------------------------------------------------

:Supports: ALL PLATFORMS (Python based)

.. code-block:: bash

    # -- PREPARE: Ensure that all required Python packages are installed.
    $ pip install -r py.requirements/all.txt

    # -- LIST ALL TASKS:
    $ invoke --list

    # -- STEP: Run the tests with "pytest".
    # OUTPUTS: build/testing/report.html, build/testing/report.xml
    $ invoke test

    # -- STEP: Determine the test.coverage and generate test reports.
    # OUTPUT: build/coverage.html/index.html
    $ invoke test.coverage

    # -- OPTIONAL: Cleanup afterwards
    # HINT: cleanup.all cleans up everything (even virtual-environments, etc.)
    $ invoke cleanup
    $ invoke cleanup.all

    # -- KNOWN PROBLEM: On Python 3.10, using "invoke" runs into a problem.
    # SEE ISSUE: #820 (on: https://github.com/pyinvoke/invoke/issues/ )


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
    $ pip install -r py.requirements/all.txt

    # -- HINT: Afterwards, to deactivate the virtual-enviroment, use:
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
    cmd> pip install -r py.requirements/all.txt

SEE ALSO:

* https://virtualenv.pypa.io/en/latest/user_guide.html


USE CASE: Without virtual-environment
-------------------------------------------------------------------------------

Ensure that all required Python packages are installed::

    $ pip install -r py.requirements/all.txt

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

    $ tox -e py39     # Run tests in a virtual environment with python3.9
    $ tox -e py27     # Run tests in a virtual environment with python2.7

SEE ALSO:

* https://tox.wiki/
* https://pypi.org/project/tox


USE CASE: Use Static Code Analyzers to detect Problems
-------------------------------------------------------------------------------

Perform checks with the following commands::

    $ pylint cucumber_tag_expressions/     # Run pylint checks.
    $ bandit cucumber_tag_expressions/     # Run bandit security checks.

ALTERNATIVE: Run tools in a tox environment::

    $ tox -e pylint         # Run pylint checks.
    $ tox -e bandit         # Run bandit security checks.

SEE ALSO:

* https://pylint.readthedocs.io/
* https://bandit.readthedocs.io/
* https://prospector.landscape.io/


USE CASE: Cleanup the Workspace
-------------------------------------------------------------------------------

To cleanup the local workspace and development environment, use::

    $ invoke cleanup        # Cleanup common temporary files.
    $ invoke cleanup.all    # Cleanup everything (.venv, .tox, ...)

or::

    $ make clean


USE CASE: Use "dotenv" to simplify Setup of Environment Variables
-------------------------------------------------------------------------------

`direnv`_ simplifies the setup and cleanup of environment variables.
If `direnv`_ is set up:

* On entering this directory: Environment variables from ``.envrc`` file are set up.
* On leaving  this directory: The former environment is restored.

OPTIONAL PARTS (currently disabled):

* ``.envrc.use_pep0528.disabled``: Support ``__pypackages__/$(PYTHON_VERSION)/`` search paths.
* ``.envrc.use_venv.disabled``: Auto-create a virtual-environment and activate it.

Each optional part can be enabled by removing the ``.disabled`` file name suffix.
EXAMPLE: Rename ``.envrc.use_venv.disabled`` to ``.envrc.use_venv`` to enable it.

SEE ALSO:

* https://direnv.net/
* https://peps.python.org/pep-0582/   -- Python local packages directory

.. _direnv: https://direnv.net/
