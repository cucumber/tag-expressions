namespace Cucumber.TagExpressions;

/// <summary>
/// Represents a tag expression that can be evaluated against a set of input tags.
/// </summary>
public interface ITagExpression
{
 /// <summary>
 /// Evaluates the tag expression against the provided input tags.
 /// </summary>
 /// <param name="inputs">A collection of input tags to evaluate against.</param>
 /// <returns><c>true</c> if the expression matches the inputs; otherwise, <c>false</c>.</returns>
 bool Evaluate(IEnumerable<string> inputs);

 /// <summary>
 /// Returns the string representation of the tag expression.
 /// </summary>
 /// <returns>A string that represents the tag expression.</returns>
 string ToString();
}
