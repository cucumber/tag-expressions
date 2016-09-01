LICENSE: ../../LICENSE
	cp $< $@

.PHONY: release
release:
	bundle exec rake build release:guard_clean release:rubygem_push
	version=$$(cat cucumber-tag_expressions.gemspec | grep -m 1 ".version *= *" | sed "s/.*= *'\([^']*\)'.*/\1/"); \
	git tag --annotate v$$version --message "Release $$version"
	git push --tags
