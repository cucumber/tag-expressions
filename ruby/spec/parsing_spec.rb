require 'cucumber/tag_expressions/parser'
require 'yaml'

tests = YAML.load_file('../testdata/parsing.yml')

describe 'Parsing' do
  tests.each do |test|
    it %Q{parses "#{test['expression']}" into "#{test['formatted']}"} do
      parser = Cucumber::TagExpressions::Parser.new
      expression = parser.parse(test['expression'])
      expect(expression.to_s).to eq(test['formatted'])
    end
  end
end
