version = File.read(File.expand_path("VERSION", __dir__)).strip

Gem::Specification.new do |s|
  s.name        = 'cucumber-tag-expressions'
  s.version     = version
  s.authors     = ['Andrea Nodari', "Aslak Hellesøy"]
  s.description = 'Cucumber tag expressions for ruby'
  s.summary     = "#{s.name}-#{s.version}"
  s.email       = 'cukes@googlegroups.com'
  s.homepage    = 'https://cucumber.io/docs/cucumber/api/#tag-expressions'
  s.platform    = Gem::Platform::RUBY
  s.license     = 'MIT'
  s.required_ruby_version = '>= 2.3'
  s.required_rubygems_version = '>= 3.0.8'

  s.metadata    = {
                    'bug_tracker_uri'   => 'https://github.com/cucumber/cucumber/issues',
                    'changelog_uri'     => 'https://github.com/cucumber/tag-expressions/blob/main/CHANGELOG.md',
                    'documentation_uri' => 'https://cucumber.io/docs/cucumber/api/#tag-expressions',
                    'mailing_list_uri'  => 'https://groups.google.com/forum/#!forum/cukes',
                    'source_code_uri'   => 'https://github.com/cucumber/tag-expressions/tree/main/ruby',
                  }

  s.add_development_dependency 'rake', '~> 13.0', '>= 13.0.6'
  s.add_development_dependency 'rspec', '~> 3.11'
  s.add_development_dependency 'rubocop', '~> 0.79.0'

  s.files            = Dir[
    'README.md',
    'LICENSE',
    'lib/**/*'
  ]
  s.test_files       = Dir['spec/**/*']
  s.rdoc_options     = ['--charset=UTF-8']
  s.require_path     = 'lib'
end
