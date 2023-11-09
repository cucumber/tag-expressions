# frozen_string_literal: true

tests = YAML.load_file('../testdata/parsing.yml')

describe 'Parsing' do
  let(:parser) { Cucumber::TagExpressions::Parser.new }

  tests.each do |test|
    it "parses '#{test['expression']}' into '#{test['formatted']}'" do
      expect(parser.parse(test['expression']).to_s).to eq(test['formatted'])
    end
  end
end
