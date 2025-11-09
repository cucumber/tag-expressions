using System.Text;

namespace Cucumber.TagExpressions;

/// <summary>
/// Provides an abstract base for tag expressions that can be evaluated against a set of input tags.
/// </summary>
public abstract class TagExpression : ITagExpression
{
    /// <summary>
    /// Evaluates the tag expression against the provided input tags.
    /// </summary>
    /// <param name="inputs">A collection of input tags to evaluate against.</param>
    /// <returns><c>true</c> if the expression matches the inputs; otherwise, <c>false</c>.</returns>
    public bool Evaluate(IEnumerable<string> inputs)
    {
        var inputSet = new HashSet<string>(inputs);
        return EvaluateInternal(inputSet);
    }

    /// <summary>
    /// Returns the string representation of the tag expression.
    /// </summary>
    /// <returns>A string that represents the tag expression.</returns>
    public abstract override string ToString();

    /// <summary>
    /// Recursively evaluates the tag expression against the provided set of input tags.
    /// </summary>
    /// <param name="inputs">A set of input tags.</param>
    /// <returns><c>true</c> if the expression matches the inputs; otherwise, <c>false</c>.</returns>
    internal abstract bool EvaluateInternal(HashSet<string> inputs);
}

/// <summary>
/// Represents an expression that always evaluates to <see langword="true"/>.
/// </summary>
/// <remarks>
/// This expression is used as a fallback condition when the expression input is empty.
/// By convention, an empty tag expression is considered equivalent to <see langword="true"/> regardless of input.
/// </remarks>
public class NullExpression : TagExpression
{
    /// <inheritdoc/>
    public override string ToString() => "";

    /// <inheritdoc/>
    internal override bool EvaluateInternal(HashSet<string> inputs) => true;
}

/// <summary>
/// Represents a leaf node for a tag expression, corresponding to a literal tag name.
/// </summary>
public class LiteralNode : TagExpression
{
    /// <summary>
    /// Gets the name of the literal tag.
    /// </summary>
    public string Name { get; }

    /// <summary>
    /// Initializes a new instance of the <see cref="LiteralNode"/> class.
    /// </summary>
    /// <param name="name">The name of the tag.</param>
    public LiteralNode(string name) => Name = name;

    /// <inheritdoc/>
    public override string ToString()
    {
        var sb = new StringBuilder();
        foreach (char c in Name)
        {
            if (c == '\\' || c == ' ' || c == '(' || c == ')')
                sb.Append('\\');
            sb.Append(c);
        }
        return sb.ToString();
    }

    /// <inheritdoc/>
    internal override bool EvaluateInternal(HashSet<string> inputs) => inputs.Contains(Name);
}

/// <summary>
/// Represents a unary NOT operation in a tag expression.
/// </summary>
public class NotNode : TagExpression
{
    /// <summary>
    /// Gets the operand of the NOT operation.
    /// </summary>
    public ITagExpression Operand { get; }

    /// <summary>
    /// Initializes a new instance of the <see cref="NotNode"/> class.
    /// </summary>
    /// <param name="operand">The operand expression to negate.</param>
    public NotNode(ITagExpression operand) => Operand = operand;

    /// <inheritdoc/>
    public override string ToString()
    {
        string operandStr = Operand is not BinaryOpNode ? $"( {Operand} )" : Operand.ToString();
        return $"not {operandStr}";
    }

    /// <inheritdoc/>
    internal override bool EvaluateInternal(HashSet<string> inputs) => !(((TagExpression)Operand).EvaluateInternal(inputs));
}

/// <summary>
/// Represents a binary operator node for AND and OR operations in a tag expression.
/// </summary>
public class BinaryOpNode : TagExpression
{
    /// <summary>
    /// Gets the left operand of the binary operation.
    /// </summary>
    public ITagExpression Left { get; }

    /// <summary>
    /// Gets the right operand of the binary operation.
    /// </summary>
    public ITagExpression Right { get; }

    /// <summary>
    /// Gets the operator for the binary operation ("AND" or "OR").
    /// </summary>
    public string Op { get; }

    /// <summary>
    /// Initializes a new instance of the <see cref="BinaryOpNode"/> class.
    /// </summary>
    /// <param name="op">The operator ("AND" or "OR").</param>
    /// <param name="left">The left operand.</param>
    /// <param name="right">The right operand.</param>
    public BinaryOpNode(string op, ITagExpression left, ITagExpression right)
    {
        Op = op;
        Left = left;
        Right = right;
    }

    /// <inheritdoc/>
    public override string ToString()
    {
        string leftStr = Left is BinaryOpNode leftBin && Precedence(leftBin.Op) < Precedence(Op)
        ? $"( {Left} )" : Left.ToString();
        string rightStr = Right is BinaryOpNode rightBin && Precedence(rightBin.Op) < Precedence(Op)
        ? $"( {Right} )" : Right.ToString();
        return $"( {leftStr} {Op} {rightStr} )";
    }

    private int Precedence(string op) =>
        op == "not" ? 3 :
        op == "and" ? 2 :
        op == "or" ? 1 : 0;

    /// <inheritdoc/>
    internal override bool EvaluateInternal(HashSet<string> inputs)
    {
        bool leftVal = ((TagExpression)Left).EvaluateInternal(inputs);
        bool rightVal = ((TagExpression)Right).EvaluateInternal(inputs);
        return Op == "and" ? (leftVal && rightVal) : (leftVal || rightVal);
    }
}
