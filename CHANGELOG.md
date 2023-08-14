# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/).

## [Unreleased]
### Fixed
- [Perl] Include README.md and LICENSE in the release tarball
(by [ehuelsmann](https://github.com/ehuelsmann))

## [5.0.6] - 2023-08-13
### Fixed
- [Perl] Fixed test failures when running tests out-of-tree by
resticting testdata based tests to run in development only
(by [ehuelsmann](https://github.com/ehuelsmann))

## [5.0.5] - 2023-08-11
### Fixed
- [Perl] Fixed inclusion of CHANGELOG.md causing release to fail
(by [ehuelsmann](https://github.com/ehuelsmann))

## [5.0.4] - 2023-08-10
### Fixed
- [Perl] Corrected working directory for CPAN upload action
(by [ehuelsmann](https://github.com/ehuelsmann))

## [5.0.3] - 2023-08-10
### Fixed
- [Perl] Fixed CPAN upload in release process
(by [ehuelsmann](https://github.com/ehuelsmann))

## [5.0.2] - 2023-07-15
### Added
- [Python] Make tests pass against shared test data (except: backslash-escaped)
([#18](https://github.com/cucumber/tag-expressions/issues/18)
by [jenisys](https://github.com/jenisys))

### Fixed
- [All] `Not.to_string()` conversion has unneeded double-parenthesis if binary operator is contained
([#94](https://github.com/cucumber/tag-expressions/issues/94)
by [jenisys](https://github.com/jenisys))

## [5.0.1] - 2023-01-03
### Fixed
- [Java] Fix scm and project urls

## [5.0.0] - 2023-01-02
### Added
- [JavaScript] Support for EcmaScript modules (aka ESM). ([#5](https://github.com/cucumber/tag-expressions/pull/5))
- [Java] Enabled reproducible builds

### Changed
- Only allow escape character `\` in front of `(`, `)`, `\` or whitespace. Throw error otherwise. ([#17](https://github.com/cucumber/tag-expressions/pull/17))

### Deprecated

### Fixed
- [Perl] Fixed missing dependency as well as new syntax in the tests
([cucumber/tag-expressions#15](https://github.com/cucumber/tag-expressions/pull/15)
[ehuelsmann](https://github.com/ehuelsmann))
- Document escaping. ([#16](https://github.com/cucumber/tag-expressions/issues/16), [#17](https://github.com/cucumber/tag-expressions/pull/17))
- [Ruby], [Perl] Empty expression evaluates to true
- [Go] Fix module name ([#82](https://github.com/cucumber/tag-expressions/pull/82))

### Removed

## [4.1.0] - 2021-10-08
### Added
- [Perl] Add new implementation
([#1782](https://github.com/cucumber/common/pull/1782) [ehuelsmann](https://github.com/ehuelsmann))

### Fixed
- [Go], [JavaScript], [Java], [Ruby] Support backslash-escape in tag expressions
([#1778](https://github.com/cucumber/common/pull/1778) [yusuke-noda](https://github.com/yusuke-noda))

## [4.0.2] - 2021-09-13
### Fixed
- [Python] Remove call to deprecated `2to3` library causing `pip install` to fail
([#1736](https://github.com/cucumber/common/issues/1736)
[krisgesling](https://github.com/krisgesling))

## [4.0.0] - 2021-09-02
### Changed
- [Go] Move module paths to point to monorepo
([#1550](https://github.com/cucumber/common/issues/1550))

## [3.0.1] - 2021-03-31
### Fixed
- Previous release 3.0.0 did not publish to npm for some reason. Re-releasing.

## [3.0.0] - 2020-06-11
### Added
- [Java] Enable consumers to find our version at runtime using `clazz.getPackage().getImplementationVersion()` by upgrading to `cucumber-parent:2.1.0`
([#976](https://github.com/cucumber/cucumber/pull/976)
[aslakhellesoy](https://github.com/aslakhellesoy))

### Changed
- [Java] Updated `TagExpressionParser` to use a static method to parse a tag expression and return an `Expression` object to the user.
- [Java] Reduced public API to the bare minimum required.
- [Java] Added more informative error messages for `TagExpressionParser` through the `TagExpressionException`.
([#1005](https://github.com/cucumber/cucumber/pull/1005)
[cyocum](https://github.com/cyocum)

## [2.0.4] - 2020-01-10
### Changed
- [JavaScript] changed module name to `@cucumber/tag-expressions`

## 2.0.3 - 2019-12-10
### Changed
- [Java] Upgrades to `cucumber-parent:2.0.2`
- [Ruby] Renamed gem to `tag-expressions`

### Removed
- [Ruby] Removed `tag-expressions` executable

## [2.0.2] - 2019-07-15
### Fixed
- Fix incomplete 2.0.1 release

## 2.0.1 - 2019-07-15
### Fixed
- Fix incomplete 2.0.0 release

## [2.0.0] - 2019-07-10
### Added
- Go: New implementation.
([#339](https://github.com/cucumber/cucumber/pull/339)
[charlierudolph](https://github.com/charlierudolph))

### Changed
- JavaScript: Changed API to return a `parse` function rather than a class with a `parse` method.
- JavaScript: Refactored to TypeScript

### Fixed
- Documentation links now point to new website (cucumber.io)
([#560](https://github.com/cucumber/cucumber/issues/560)
[luke-hill](https://github.com/luke-hill))

### Removed
- Java: OSGi support has been removed.
([#412](https://github.com/cucumber/cucumber/issues/412)
[aslakhellesoy](https://github.com/aslakhellesoy))

## [1.1.1] - 2017-12-01
### Fixed
- Java: Fix OSGI exported package
([#309](https://github.com/cucumber/cucumber/pull/309)
by [mpkorstanje](https://github.com/mpkorstanje))

## [1.1.0] - 2017-11-28
### Added
- Ruby: Added `tag-expressions` command-line tool for tag expressions
([#282](https://github.com/cucumber/cucumber/pull/282)
by [aslakhellesoy](https://github.com/aslakhellesoy))
- Escape special chars in tags
([#286](https://github.com/cucumber/cucumber/pull/286)
[#285](https://github.com/cucumber/cucumber/issues/285)
by [link89](https://github.com/link89))

### Fixed
- Don't support RPN
([#304](https://github.com/cucumber/cucumber/issues/304)
by [aslakhellesoy](https://github.com/aslakhellesoy))
- Parse empty tag expressions (always evaluates to true)
([#296](https://github.com/cucumber/cucumber/issues/296)
by [aslakhellesoy](https://github.com/aslakhellesoy))

## [1.0.1] - 2017-05-28
### Fixed
- javascript:
([#76](https://github.com/cucumber/cucumber/pull/76)
[#78](https://github.com/cucumber/cucumber/pull/78)
[#104](https://github.com/cucumber/cucumber/issues/104)
by [charlierudolph](https://github.com/charlierudolph))
- java: Make the jar a bundle to support OSGi
([#99](https://github.com/cucumber/cucumber/pull/99)
by [brasmusson](https://github.com/brasmusson))
- Add a [changelog](keepachangelog.com)
([#213](https://github.com/cucumber/cucumber/issues/213)
by [aslakhellesoy](https://github.com/aslakhellesoy))

## [1.0.0] - 2016-09-01
### Added
- First stable release!

[Unreleased]: https://github.com/cucumber/tag-expressions/compare/v5.0.6...HEAD
[5.0.6]: https://github.com/cucumber/tag-expressions/compare/v5.0.5...v5.0.6
[5.0.5]: https://github.com/cucumber/tag-expressions/compare/v5.0.4...v5.0.5
[5.0.4]: https://github.com/cucumber/tag-expressions/compare/v5.0.3...v5.0.4
[5.0.3]: https://github.com/cucumber/tag-expressions/compare/v5.0.2...v5.0.3
[5.0.2]: https://github.com/cucumber/tag-expressions/compare/v5.0.1...v5.0.2
[5.0.1]: https://github.com/cucumber/tag-expressions/compare/v5.0.0...v5.0.1
[5.0.0]: https://github.com/cucumber/tag-expressions/compare/v4.1.0...v5.0.0
[4.1.0]: https://github.com/cucumber/tag-expressions/compare/v4.0.2...v4.1.0
[4.0.2]: https://github.com/cucumber/tag-expressions/compare/v4.0.0...v4.0.2
[4.0.0]: https://github.com/cucumber/tag-expressions/compare/v3.0.1...v4.0.0
[3.0.1]: https://github.com/cucumber/tag-expressions/compare/v3.0.0...v3.0.1
[3.0.0]: https://github.com/cucumber/tag-expressions/compare/v2.0.4...v3.0.0
[2.0.4]: https://github.com/cucumber/tag-expressions/compare/v2.0.2...v2.0.4
[2.0.2]: https://github.com/cucumber/tag-expressions/compare/v2.0.0...v2.0.2
[2.0.0]: https://github.com/cucumber/tag-expressions/compare/v1.1.1...v2.0.0
[1.1.1]: https://github.com/cucumber/tag-expressions/compare/v1.1.0...v1.1.1
[1.1.0]: https://github.com/cucumber/tag-expressions/compare/v1.0.1...v1.1.0
[1.0.1]: https://github.com/cucumber/tag-expressions/compare/v1.0.0...v1.0.1
[1.0.0]: https://github.com/cucumber/tag-expressions/releases/tag/v1.0.0
