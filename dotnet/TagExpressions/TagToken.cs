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
    public string Value { get; }

    public TagToken(TagTokenType type, string value = null)
    {
        Type = type;
        Value = value;
    }
}  
