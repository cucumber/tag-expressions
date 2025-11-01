using System.Text;

namespace TagExpressions;

// Recursive descent parser for logical expressions
public class TagExpressionParser : ITagExpressionParser
{
    private string _text;
    private TagLexer _lexer;
    private TagToken _current;
    private int _openParens;

    public ITagExpression Parse(string text)
    {
        _text = text;
        _openParens = 0;
        _lexer = new TagLexer(text);
        Next();
        if (_current.Type == TagTokenType.End)
            return new NullExpression();

        var expr = ParseExpression();

        while (_current.Type != TagTokenType.End)
        {
            Next();
        }
        if (_openParens != 0)
            ThrowSyntaxError("Unmatched (");
        return expr;
    }

    private void Next()
    {
        _current = _lexer.NextToken();
        if (_current.Type == TagTokenType.LParen)
            _openParens++;
        else if (_current.Type == TagTokenType.RParen)
        {
            _openParens--;
            if (_openParens < 0)
                ThrowSyntaxError("Unmatched )");
        }
    }

    private void ThrowSyntaxError(string message)
    {
        throw new Exception($"Tag expression \"{_text}\" could not be parsed because of syntax error: {message}.");
    }

    // Expression := Term { OR Term }
    private ITagExpression ParseExpression()
    {
        var left = ParseTerm();

        if (_current.Type != TagTokenType.Or &&
            _current.Type != TagTokenType.RParen &&
            _current.Type != TagTokenType.End)
        {
            ThrowSyntaxError("Expected operator");
        }

        while (_current.Type == TagTokenType.Or)
        {
            Next();
            var right = ParseTerm();
            left = new BinaryOpNode("or", left, right);
        }
        return left;
    }


    // Term := Factor { AND Factor }
    private ITagExpression ParseTerm()
    {
        var left = ParseFactor();

        if (_current.Type != TagTokenType.Or &&
            _current.Type != TagTokenType.And &&
            _current.Type != TagTokenType.RParen &&
            _current.Type != TagTokenType.End)
        {
            ThrowSyntaxError("Expected operator");
        }

        while (_current.Type == TagTokenType.And)
        {
            Next();
            var right = ParseFactor();
            left = new BinaryOpNode("and", left, right);
        }
        return left;
    }

    // Factor := NOT Factor | (Expression) | Identifier
    private ITagExpression ParseFactor()
    {
        switch (_current.Type)
        {
            case TagTokenType.Not:
                Next();
                // Only NOT, (, or Identifier are valid after NOT
                if (_current.Type != TagTokenType.Not &&
                    _current.Type != TagTokenType.LParen &&
                    _current.Type != TagTokenType.Identifier)
                {
                    ThrowSyntaxError("Expected operand");
                }
                var operand = ParseFactor();
                return new NotNode(operand);

            case TagTokenType.LParen:
                Next();
                var expr = ParseExpression();
                if (_current.Type != TagTokenType.RParen)
                    ThrowSyntaxError("Unmatched (");
                Next();
                return expr;

            case TagTokenType.Identifier:
                var ident = _current.Value;
                Next();
                return new LiteralNode(ident);

            default:
                ThrowSyntaxError("Expected operand");
                return null; // unreachable
        }
    }
}
