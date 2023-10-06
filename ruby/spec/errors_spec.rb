require 'cucumber/tag_expressions/parser'
require 'yaml'

tests = YAML.load_file('../testdata/errors.yml')

describe 'Errors' do
  tests.each do |test|
    let(:parser) { Cucumber::TagExpressions::Parser.new }

    it %Q{fails to parse "#{test['expression']}" with "#{test['error']}"} do
      expect { parser.parse(test['expression']) }.to raise_error(test['error'])
    end
  end
end
