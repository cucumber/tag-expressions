include default.mk

test: .cpanfile_dependencies
	AUTHOR_TESTS=1 prove -l
.PHONY: test

authortest: .cpanfile_dev_dependencies
	AUTHOR_TESTS=1 prove -l
.PHONY: authortest

clean:
	rm -rf Cucumber-* .cpanfile_dependencies .built CHANGELOG.md
.PHONY: clean
