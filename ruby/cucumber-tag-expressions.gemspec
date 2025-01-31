# frozen_string_literal: true

version = File.read(File.expand_path('VERSION', __dir__)).strip

Gem::Specification.new do |s|
  s.name        = 'cucumber-tag-expressions'
  s.version     = version
  s.authors     = ['Andrea Nodari', 'Aslak Hellesøy']
  s.description = 'Cucumber tag expressions for ruby'
  s.summary     = "#{s.name}-#{s.version}"
  s.email       = 'cukes@googlegroups.com'
  s.homepage    = 'https://cucumber.io/docs/cucumber/api/#tag-expressions'
  s.platform    = Gem::Platform::RUBY
  s.license     = 'MIT'
  s.required_ruby_version = '>= 2.6'
  s.required_rubygems_version = '>= 3.2.3'

  s.metadata = {
    'bug_tracker_uri' => 'https://github.com/cucumber/cucumber/issues',
    'changelog_uri' => 'https://github.com/cucumber/tag-expressions/blob/main/CHANGELOG.md',
    'documentation_uri' => 'https://cucumber.io/docs/cucumber/api/#tag-expressions',
    'mailing_list_uri' => 'https://groups.google.com/forum/#!forum/cukes',
    'source_code_uri' => 'https://github.com/cucumber/tag-expressions/tree/main/ruby'
  }

  s.add_development_dependency 'rake', '~> 13.2'
  s.add_development_dependency 'rspec', '~> 3.13'
  s.add_development_dependency 'rubocop', '~> 1.18.4'

  s.files = Dir['README.md', 'LICENSE', 'lib/**/*']
  s.rdoc_options     = ['--charset=UTF-8']
  s.require_path     = 'lib'
end
