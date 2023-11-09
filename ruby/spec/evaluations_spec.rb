# frozen_string_literal: true

evaluations = YAML.load_file('../testdata/evaluations.yml')

describe 'Evaluations' do
  evaluations.each do |evaluation|
    context evaluation['expression'] do
      let(:parser) { Cucumber::TagExpressions::Parser.new }

      evaluation['tests'].each do |test|
        it "evaluates [#{test['variables'].join(', ')}] to #{test['result']}" do
          expect(parser.parse(evaluation['expression']).evaluate(test['variables'])).to eq(test['result'])
        end
      end
    end
  end
end
