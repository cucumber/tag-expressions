require 'cucumber/tag_expressions/parser'
require 'yaml'

tests = YAML.load_file('../testdata/errors.yml')

describe 'Errors' do
  tests.each do |test|
    it %Q{fails to parse "#{test['expression']}" with "#{test['error']}"} do
      parser = Cucumber::TagExpressions::Parser.new
      expect { parser.parse(test['expression']) }.to raise_error(test['error'])
    end
  end
end
