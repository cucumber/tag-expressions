require 'cucumber/tag_expressions/parser'
require 'yaml'

evaluations = YAML.load_file('../testdata/evaluations.yml')

describe 'Evaluations' do
  evaluations.each do |evaluation|
    context evaluation['expression'] do
      evaluation['tests'].each do |test|
        it "evaluates [#{test['variables'].join(', ')}] to #{test['result']}" do
          parser = Cucumber::TagExpressions::Parser.new
          expect(parser.parse(evaluation['expression']).evaluate(test['variables'])).to eq(test['result'])
        end
      end
    end
  end
end
