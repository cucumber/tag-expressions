# frozen_string_literal: true

require 'cucumber/tag_expressions/expressions.rb'

module Cucumber
  module TagExpressions
    class Parser
      def initialize
        @expressions = []
        @operators = []

        @operator_types = {
          'or' => { type: :binary_operator,    precedence: 0, assoc: :left },
          'and' => { type: :binary_operator,   precedence: 1, assoc: :left },
          'not' => { type: :unary_operator,   precedence: 2, assoc: :right },
          ')' => { type: :close_paren,       precedence: -1 },
          '(' => { type: :open_paren,        precedence: 1 }
        }
      end

      def parse(infix_expression)
        expected_token_type = :operand

        tokens = tokenize(infix_expression)
        return True.new if tokens.empty?

        tokens.each do |token|
          if @operator_types[token]
            expected_token_type = send("handle_#{@operator_types[token][:type]}", infix_expression, token, expected_token_type)
          else
            expected_token_type = handle_literal(infix_expression, token, expected_token_type)
          end
        end

        while @operators.any?
          raise %Q{Tag expression "#{infix_expression}" could not be parsed because of syntax error: Unmatched (.} if @operators.last == '('

          push_expression(pop(@operators))
        end
        expression = pop(@expressions)
        @expressions.empty? ? expression : raise('Not empty')
      end

      private

      def assoc_of(token, value)
        @operator_types[token][:assoc] == value
      end

      def lower_precedence?(operation)
        (assoc_of(operation, :left) &&
         precedence(operation) <= precedence(@operators.last)) ||
          (assoc_of(operation, :right) &&
           precedence(operation) < precedence(@operators.last))
      end

      def operator?(token)
        @operator_types[token][:type] == :unary_operator ||
            @operator_types[token][:type] == :binary_operator
      end

      def precedence(token)
        @operator_types[token][:precedence]
      end

      def tokenize(infix_expression)
        tokens = []
        escaped = false
        token = ""
        infix_expression.chars.each do | ch |
          if escaped
            if ch == '(' || ch == ')' || ch == '\\' || ch.match(/\s/)
              token += ch
              escaped = false
            else
              raise %Q{Tag expression "#{infix_expression}" could not be parsed because of syntax error: Illegal escape before "#{ch}".}
            end
          elsif ch == '\\'
            escaped = true
          elsif ch == '(' || ch == ')' || ch.match(/\s/)
            if token.length > 0
              tokens.push(token)
              token = ""
            end
            if !ch.match(/\s/)
              tokens.push(ch)
            end
          else
            token += ch
          end
        end
        if token.length > 0
          tokens.push(token)
        end
        tokens
      end

      def push_expression(token)
        case token
        when 'and'
          @expressions.push(And.new(*pop(@expressions, 2)))
        when 'or'
          @expressions.push(Or.new(*pop(@expressions, 2)))
        when 'not'
          @expressions.push(Not.new(pop(@expressions)))
        else
          @expressions.push(Literal.new(token))
        end
      end

      def handle_unary_operator(infix_expression, token, expected_token_type)
        check(infix_expression, expected_token_type, :operand)
        @operators.push(token)
        :operand
      end

      def handle_binary_operator(infix_expression, token, expected_token_type)
        check(infix_expression, expected_token_type, :operator)
        while @operators.any? && operator?(@operators.last) &&
              lower_precedence?(token)
          push_expression(pop(@operators))
        end
        @operators.push(token)
        :operand
      end

      def handle_open_paren(infix_expression, token, expected_token_type)
        check(infix_expression, expected_token_type, :operand)
        @operators.push(token)
        :operand
      end

      def handle_close_paren(infix_expression, _token, expected_token_type)
        check(infix_expression, expected_token_type, :operator)
        while @operators.any? && @operators.last != '('
          push_expression(pop(@operators))
        end
        raise %Q{Tag expression "#{infix_expression}" could not be parsed because of syntax error: Unmatched ).} if @operators.empty?

        pop(@operators) if @operators.last == '('
        :operator
      end

      def handle_literal(infix_expression, token, expected_token_type)
        check(infix_expression, expected_token_type, :operand)
        push_expression(token)
        :operator
      end

      def check(infix_expression, expected_token_type, token_type)
        if expected_token_type != token_type
          raise %Q{Tag expression "#{infix_expression}" could not be parsed because of syntax error: Expected #{expected_token_type}.}
        end
      end

      def pop(array, n = 1)
        result = array.pop(n)
        raise('Empty stack') if result.size != n

        n == 1 ? result.first : result
      end
    end
  end
end
