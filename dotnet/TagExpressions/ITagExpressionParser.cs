namespace TagExpressions
{
    public interface ITagExpressionParser
    {
        ITagExpression Parse(string text);
    }
}
