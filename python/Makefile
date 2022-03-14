# =============================================================================
# GNU MAKEFILE: Cucumber tag-expressions for Python
# =============================================================================
# NOTE: install-packages/bootstrap requires ON-LINE access.
# REQUIRES: Python >= 2.7 is installed
# DESCRIPTION:
#    This makefile uses "pip" to automatically install the required packages:
#
#    * CASE 1: If a Python a virtual environment was set up and activated,
#      the packages are installed into the virtual environment.
#
#    * CASE 2: The packages are installed in the $HOME directory.
#      On macOS: $HOME/Library/Python/$(PYTHON_VERSION)/lib
#      HINT: May need PATH="$HOME/Library/Python/$(PYTHON_VERSION)/bin:${PATH}"
#
# SEE ALSO:
#  * https://pip.pypa.io/en/stable/
# =============================================================================

.PHONY: clean clean-all default test test.coverage tox

# -----------------------------------------------------------------------------
# PROJECT CONFIGURATION:
# -----------------------------------------------------------------------------
PYTHON ?= python3
PYTHON_VERSION := $(shell $(PYTHON) -c "import sys; print(sys.version.split()[0])")
PIP := $(PYTHON) -mpip
PIP_INSTALL_OPTIONS ?= --quiet
PIP_RUN :=

PY_REQUIREMENT_FILES := py.requirements/all.txt

INSTALL_PACKAGES_DONE_MARKER_FILE := build/.done.install_packages.python_v$(PYTHON_VERSION)
INSTALL_PACKAGES_DONE_MARKER_DIR  := $(dir $(INSTALL_PACKAGES_DONE_MARKER_FILE))

PYTHONPATH ?= $(CURDIR)
export PYTHONPATH

# -----------------------------------------------------------------------------
# MAKE RULES:
# -----------------------------------------------------------------------------
.PHONY: all clean install-packages test test.coverage tox

# -- DIAGNOSTICS:
$(info USING: PYTHON_VERSION=$(PYTHON_VERSION))
$(info USING: PYTHONPATH=$(PYTHONPATH))

all: .ensure.packages_are_installed test

.ensure.packages_are_installed: $(INSTALL_PACKAGES_DONE_MARKER_FILE)
install-packages $(INSTALL_PACKAGES_DONE_MARKER_FILE): $(PY_REQUIREMENT_FILES)
	@echo "INSTALL-PACKAGES: ..."
	test -d $(INSTALL_PACKAGES_DONE_MARKER_DIR) || mkdir -p $(INSTALL_PACKAGES_DONE_MARKER_DIR)
	$(PIP) install $(PIP_INSTALL_OPTIONS) $(addprefix -r ,$(PY_REQUIREMENT_FILES))
	touch $(INSTALL_PACKAGES_DONE_MARKER_FILE)
	@echo "INSTALL-PACKAGES: done"
	@echo ""

clean:
	-rm -f $(INSTALL_PACKAGES_DONE_MARKER_FILE)
	-rm -rf build dist .tox .venv* .dotenv/
	-rm -rf get-pip.py
	-py.cleanup

test: $(INSTALL_PACKAGES_DONE_MARKER_FILE)
	$(PIP_RUN) pytest $(PYTEST_ARGS)

test.coverage: $(INSTALL_PACKAGES_DONE_MARKER_FILE)
	$(PIP_RUN) coverage run -m pytest $(PYTEST_ARGS); coverage combine; coverage report; coverage html

tox: $(INSTALL_PACKAGES_DONE_MARKER_FILE)
	$(PIP_RUN) tox $(TOX_ARGS)

include default.mk

# -- ADD DEPENDENCY:
update-dependencies: .ensure.packages_are_installed
