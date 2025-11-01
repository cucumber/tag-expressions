using System.Text;

namespace TagExpressions;

/// <summary>
/// Provides a recursive descent parser for logical tag expressions.
/// </summary>
public class TagExpressionParser : ITagExpressionParser
{
    private string _text;
    private TagLexer _lexer;
    private TagToken _current;
    private int _openParens;

    /// <summary>
    /// Parses the specified tag expression string into an <see cref="ITagExpression"/>.
    /// </summary>
    /// <param name="text">The tag expression string to parse.</param>
    /// <returns>An <see cref="ITagExpression"/> representing the parsed expression.</returns>
    /// <exception cref="Exception">Thrown when a syntax error is encountered in the tag expression.</exception>
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

    /// <summary>
    /// Advances to the next token in the input stream.
    /// </summary>
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

    /// <summary>
    /// Throws a syntax error exception with the specified message.
    /// </summary>
    /// <param name="message">The error message.</param>
    /// <exception cref="Exception">Always thrown to indicate a syntax error.</exception>
    private void ThrowSyntaxError(string message)
    {
        throw new Exception($"Tag expression \"{_text}\" could not be parsed because of syntax error: {message}.");
    }

    /// <summary>
    /// Parses an expression consisting of terms separated by the OR operator.
    /// </summary>
    /// <returns>The parsed <see cref="ITagExpression"/>.</returns>
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

    /// <summary>
    /// Parses a term consisting of factors separated by the AND operator.
    /// </summary>
    /// <returns>The parsed <see cref="ITagExpression"/>.</returns>
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

    /// <summary>
    /// Parses a factor, which can be a NOT operation, a parenthesized expression, or an identifier.
    /// </summary>
    /// <returns>The parsed <see cref="ITagExpression"/>.</returns>
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
