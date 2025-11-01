namespace TagExpressions;

// Public interface exposing evaluation and stringification
public interface ITagExpression
{
    bool Evaluate(IEnumerable<string> inputs);
    string ToString();
}
