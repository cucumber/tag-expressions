using System;
using System.Collections.Generic;
using System.Text;

namespace TagExpressions;

// Abstract base class implementing the interface
public abstract class TagExpression : ITagExpression
{
    // Public API
    public bool Evaluate(IEnumerable<string> inputs)
    {
        var inputSet = new HashSet<string>(inputs);
        return EvaluateInternal(inputSet);
    }

    public abstract override string ToString();

    // Internal recursive evaluation method
    internal abstract bool EvaluateInternal(HashSet<string> inputs);
}

/// <summary>
/// Represents an expression that always evaluates to <see langword="true"/>.
/// </summary>
/// <remarks>This expression is used as a fallback condition when the expression input is empty.
/// By convention, an empty tag expression is considered equivalent to <see langword="true"/> regardless of input.</remarks>
public class NullExpression : TagExpression
{
    public override string ToString() => "true";
    internal override bool EvaluateInternal(HashSet<string> inputs) => true;
}

// Leaf node representing literals (variables)
public class LiteralNode : TagExpression
{
    public string Name { get; }
    public LiteralNode(string name) => Name = name;

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

    internal override bool EvaluateInternal(HashSet<string> inputs) => inputs.Contains(Name);
}

// Unary NOT node
public class NotNode : TagExpression
{
    public ITagExpression Operand { get; }
    public NotNode(ITagExpression operand) => Operand = operand;

    public override string ToString()
    {
        string operandStr = Operand is not BinaryOpNode ? $"( {Operand} )" : Operand.ToString();
        return $"not {operandStr}";
    }

    internal override bool EvaluateInternal(HashSet<string> inputs) => !( ((TagExpression) Operand).EvaluateInternal(inputs) );
}

// Binary operator node for AND / OR
public class BinaryOpNode : TagExpression
{
    public ITagExpression Left { get; }
    public ITagExpression Right { get; }
    public string Op { get; } // "AND" or "OR"

    public BinaryOpNode(string op, ITagExpression left, ITagExpression right)
    {
        Op = op;
        Left = left;
        Right = right;
    }

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

    internal override bool EvaluateInternal(HashSet<string> inputs)
    {
        bool leftVal = ((TagExpression)Left).EvaluateInternal(inputs);
        bool rightVal = ((TagExpression)Right).EvaluateInternal(inputs);
        return Op == "and" ? (leftVal && rightVal) : (leftVal || rightVal);
    }
}
