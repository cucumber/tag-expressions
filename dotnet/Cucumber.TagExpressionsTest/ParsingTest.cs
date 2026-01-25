using Cucumber.TagExpressions;

namespace Cucumber.TagExpressionsTest;

[TestClass]
public sealed class ParsingTest
{
    public class Expectation : Dictionary<string, string>
    {
        public override string ToString()
        {
            return $"{this["expression"]}: {this["formatted"]}";
        }
    }
    public static IEnumerable<object?[]> Expectations()
    {
        var folder = TestFolderHelper.TestFolder;
        var filePath = Path.Combine(folder, "parsing.yml");
        var fileContent = File.ReadAllText(filePath);
        var deserializer = new YamlDotNet.Serialization.Deserializer();
        var items = deserializer.Deserialize<List<Expectation>>(fileContent);
        foreach (var item in items)
        {
            yield return new object?[] { item };
        }
    }

    [TestMethod]
    [DynamicData(nameof(Expectations), DynamicDataSourceType.Method)]
    public void ParsedExpression_ToString_MatchesOriginalInput(Expectation expectation)
    {
        var expression = expectation["expression"];
        var formatted = expectation["formatted"];
        var parser = new TagExpressionParser();
        var parsed = parser.Parse(expression);
        Assert.AreEqual(formatted, parsed.ToString());
    }
}
