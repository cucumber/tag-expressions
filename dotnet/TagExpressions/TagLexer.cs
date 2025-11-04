using System.Collections.Generic;
using System.Text;

namespace Cucumber.TagExpressions;
internal class TagLexer
{
    private readonly string _text;
    private int _pos;
    private int _peekPos = -1;
    private TagToken? _peekedToken = null;
    private static readonly HashSet<string> Operators = new() { "AND", "OR", "NOT" };
    private static readonly char[] Escapable = new[] { '\\', ' ', '(', ')' };

    public TagLexer(string text)
    {
        _text = String.IsNullOrEmpty(text) ? "" : text;
        _pos = 0;
    }

    private void ThrowSyntaxError(string message, TagToken? tagToken)
    {
        throw new TagExpressionException($"Tag expression \"{_text}\" could not be parsed because of syntax error: {message}.", tagToken);
    }

    public TagToken NextToken()
    {
        if (_peekedToken != null)
        {
            var token = _peekedToken;
            _pos = _peekPos;
            _peekedToken = null;
            _peekPos = -1;
            return token;
        }

        return ReadToken(ref _pos);
    }

    public TagToken PeekToken()
    {
        if (_peekedToken != null)
            return _peekedToken;

        int tempPos = _pos;
        var token = ReadToken(ref tempPos);
        _peekedToken = token;
        _peekPos = tempPos;
        return token;
    }

    private TagToken ReadToken(ref int pos)
    {
        SkipWhitespace(ref pos);
        if (pos >= _text.Length)
            return new TagToken(TagTokenType.End, null, pos);

        char c = _text[pos];

        // Parentheses
        if (c == '(')
        {
            pos++;
            return new TagToken(TagTokenType.LParen, c.ToString(), pos - 1);
        }
        if (c == ')')
        {
            pos++;
            return new TagToken(TagTokenType.RParen, c.ToString(), pos - 1);
        }

        // Operators 
        var location = pos;
        var op = Operators
            .FirstOrDefault(o => _text.Substring(location).StartsWith(o, StringComparison.OrdinalIgnoreCase));
        if (op != null)
        {
            pos += op.Length;
            return new TagToken(op switch
                {
                    "AND" => TagTokenType.And,
                    "OR" => TagTokenType.Or,
                    "NOT" => TagTokenType.Not,
                    _ => throw new Exception("Unknown operator")
                },
                op,
                location);
        }

        // Identifier (with escapes)
        var sb = new StringBuilder();
        var startPos = pos;
        while (pos < _text.Length)
        {
            c = _text[pos];
            if (char.IsWhiteSpace(c) || c == '(' || c == ')')
                break;

            if (c == '\\')
            {
                if (pos + 1 < _text.Length && Escapable.Contains(_text[pos + 1]))
                {
                    pos++;
                    sb.Append(_text[pos]);
                    pos++;
                    continue;
                }
                else
                {
                    ThrowSyntaxError($"Illegal escape before \"{_text[pos + 1]}\"", new TagToken(TagTokenType.Identifier, sb.ToString(), startPos));
                }
            }

            sb.Append(c);
            pos++;
        }
        if (sb.Length > 0)
            return new TagToken(TagTokenType.Identifier, sb.ToString(), startPos);

        throw new TagExpressionException($"Unexpected character '{c}' at position {pos}");
    }

    private void SkipWhitespace(ref int pos)
    {
        while (pos < _text.Length && char.IsWhiteSpace(_text[pos]))
            pos++;
    }
}
