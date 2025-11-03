using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using YamlDotNet.Serialization;
using YamlDotNet.Serialization.NamingConventions;

namespace Cucumber.TagExpressionsTest;

[TestClass]
public class EvaluationsTest
{
    public static IEnumerable<object?[]> Expectations()
    {
        var folder = TestFolderHelper.TestFolder;
        var filePath = Path.Combine(folder, "evaluations.yml");
        var fileContent = File.ReadAllText(filePath);
        var deserializer = new DeserializerBuilder()
            .WithNamingConvention(CamelCaseNamingConvention.Instance)
            .Build();
        var testCases = deserializer.Deserialize<List<TestCase>>(fileContent);
        foreach (var item in testCases)
        {
            foreach (var test in item.Tests)
            {
                yield return new object?[] { new Expectation { Expression = item.Expression, Result = test.Result, Variables = test.Variables} };
            }
        }
    }

    [TestMethod]
    [DynamicData(nameof(Expectations), DynamicDataSourceType.Method)]
    public void EvaluatedExpression_MatchesExpectedResult(Expectation expectation)
    {
        var parser = new TagExpressions.TagExpressionParser();
        var expression = parser.Parse(expectation.Expression);
        var result = expression.Evaluate(expectation.Variables);
        Assert.AreEqual(expectation.Result, result, $"Expression: {expectation.Expression}");
    }

}
public class TestCase
{
    public string Expression { get; set; } = string.Empty;
    public List<Test> Tests { get; set; } = new List<Test>();
}
public class Test
{
    public List<string> Variables { get; set; } = new List<string>();
    public bool Result { get; set; }
}
public class Expectation
{
    public string Expression { get; set; } = string.Empty;
    public List<string> Variables { get; set; } = new List<string>();
    public bool Result { get; set; }
}
