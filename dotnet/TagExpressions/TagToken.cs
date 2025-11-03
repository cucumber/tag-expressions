namespace Cucumber.TagExpressions;

public enum TagTokenType
{
    Identifier,
    And,
    Or,
    Not,
    LParen,
    RParen,
    End
}

public class TagToken
{
    public TagTokenType Type { get; }
    public string? Value { get; }

    public int Position { get; set; }

    public TagToken(TagTokenType type, string? value = null, int position = 0)
    {
        Type = type;
        Value = value;
        Position = position;
    }
}  
