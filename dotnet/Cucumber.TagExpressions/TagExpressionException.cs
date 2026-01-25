namespace Cucumber.TagExpressions;

public class TagExpressionException : ApplicationException
{
    public TagToken? TagToken { get; private set; }

    public TagExpressionException(string message, TagToken? tagToken = null) : base(message)
    {
        TagToken = tagToken;
    }
}
