# frozen_string_literal: true

require 'cucumber/tag_expressions/expressions'

module Cucumber
  module TagExpressions
    class Parser
      def initialize
        @expressions = []
        @operators = []
      end

      def parse(infix_expression)
        expected_token_type = :operand
        tokens = tokenize(infix_expression)
        return True.new if tokens.empty?

        tokens.each { |token| expected_token_type = handle_sequential_tokens(token, infix_expression, expected_token_type) }
        while @operators.any?
          raise "Tag expression \"#{infix_expression}\" could not be parsed because of syntax error: Unmatched (." if @operators.last == '('
          push_expression(infix_expression, @operators.pop)
        end
        @expressions.pop
      end

      private

      def lower_precedence?(operation)
        (operator_types.dig(operation, :assoc) == :left && precedence(operation) <= precedence(@operators.last)) ||
          (operator_types.dig(operation, :assoc) == :right && precedence(operation) < precedence(@operators.last))
      end

      def operator?(token)
        %i[unary_operator binary_operator].include?(operator_types.dig(token, :type))
      end

      def precedence(token)
        operator_types.dig(token, :precedence)
      end

      def tokenize(infix_expression)
        tokens = []
        escaped = false
        token = +''
        infix_expression.chars.each do |char|
          if escaped
            unless char == '(' || char == ')' || char == '\\' || whitespace?(char)
              raise "Tag expression \"#{infix_expression}\" could not be parsed because of syntax error: Illegal escape before \"#{char}\"."
            end

            token += char
            escaped = false
          elsif char == '\\'
            escaped = true
          elsif char == '(' || char == ')' || whitespace?(char)
            unless token.empty?
              tokens.push(token)
              token = +''
            end
            tokens.push(char) unless whitespace?(char)
          else
            token += char
          end
        end
        tokens.push(token) unless token.empty?
        tokens
      end

      def push_expression(infix_expression, token)
        case token
        when 'and' then @expressions.push(And.new(*pop(infix_expression, @expressions, 2)))
        when 'or'  then @expressions.push(Or.new(*pop(infix_expression, @expressions, 2)))
        when 'not' then @expressions.push(Not.new(pop(infix_expression, @expressions)))
        else            @expressions.push(Literal.new(token))
        end
      end

      def handle_sequential_tokens(token, infix_expression, expected_token_type)
        if operator_types[token]
          send("handle_#{operator_types.dig(token, :type)}", infix_expression, token, expected_token_type)
        else
          handle_literal(infix_expression, token, expected_token_type)
        end
      end

      def handle_unary_operator(infix_expression, token, expected_token_type)
        check(infix_expression, expected_token_type, :operand)
        @operators.push(token)
        :operand
      end

      def handle_binary_operator(infix_expression, token, expected_token_type)
        check(infix_expression, expected_token_type, :operator)
        push_expression(infix_expression, @operators.pop) while @operators.any? && operator?(@operators.last) && lower_precedence?(token)
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
        push_expression(infix_expression, @operators.pop) while @operators.any? && @operators.last != '('
        raise "Tag expression \"#{infix_expression}\" could not be parsed because of syntax error: Unmatched )." if @operators.empty?

        @operators.pop if @operators.last == '('
        :operator
      end

      def handle_literal(infix_expression, token, expected_token_type)
        check(infix_expression, expected_token_type, :operand)
        push_expression(infix_expression, token)
        :operator
      end

      def check(infix_expression, expected_token_type, token_type)
        return if expected_token_type == token_type

        raise "Tag expression \"#{infix_expression}\" could not be parsed because of syntax error: Expected #{expected_token_type}."
      end

      def pop(infix_expression, array, amount = 1)
        result = array.pop(amount)
        raise "Tag expression \"#{infix_expression}\" could not be parsed because of syntax error: Expression is incomplete." if result.length != amount

        amount == 1 ? result.first : result
      end

      def operator_types
        {
          'or' => { type: :binary_operator, precedence: 0, assoc: :left },
          'and' => { type: :binary_operator, precedence: 1, assoc: :left },
          'not' => { type: :unary_operator, precedence: 2, assoc: :right },
          ')' => { type: :close_paren, precedence: -1 },
          '(' => { type: :open_paren, precedence: 1 }
        }
      end

      def whitespace?(char)
        char.match(/\s/)
      end
    end
  end
end
