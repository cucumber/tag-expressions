using System.Collections.Generic;
using System.Text;

internal class TagLexer
{
    private readonly string _text;
    private int _pos;
    private int _peekPos = -1;
    private TagToken _peekedToken = null;
    private static readonly HashSet<string> Operators = new() { "AND", "OR", "NOT" };
    private static readonly char[] Escapable = new[] { '\\', ' ', '(', ')' };

    public TagLexer(string text)
    {
        _text = String.IsNullOrEmpty(text) ? "" : text;
        _pos = 0;
    }

    private void ThrowSyntaxError(string message)
    {
        throw new Exception($"Tag expression \"{_text}\" could not be parsed because of syntax error: {message}.");
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
            return new TagToken(TagTokenType.End);

        char c = _text[pos];

        // Parentheses
        if (c == '(')
        {
            pos++;
            return new TagToken(TagTokenType.LParen);
        }
        if (c == ')')
        {
            pos++;
            return new TagToken(TagTokenType.RParen);
        }

        // Operators
        foreach (var op in Operators)
        {
            if (_text.Substring(pos).StartsWith(op, StringComparison.OrdinalIgnoreCase))
            {
                pos += op.Length;
                return new TagToken(op switch
                {
                    "AND" => TagTokenType.And,
                    "OR" => TagTokenType.Or,
                    "NOT" => TagTokenType.Not,
                    _ => throw new Exception("Unknown operator")
                });
            }
        }

        // Identifier (with escapes)
        var sb = new StringBuilder();
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
                    ThrowSyntaxError($"Illegal escape before \"{_text[pos + 1]}\"");
                }
            }

            sb.Append(c);
            pos++;
        }
        if (sb.Length > 0)
            return new TagToken(TagTokenType.Identifier, sb.ToString());

        throw new Exception($"Unexpected character '{c}' at position {pos}");
    }

    private void SkipWhitespace(ref int pos)
    {
        while (pos < _text.Length && char.IsWhiteSpace(_text[pos]))
            pos++;
    }
}
