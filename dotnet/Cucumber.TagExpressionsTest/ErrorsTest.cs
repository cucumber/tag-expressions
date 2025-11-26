using Cucumber.TagExpressions;

namespace Cucumber.TagExpressionsTest;

[TestClass]
public class ErrorsTest
{
    public class Expectation : Dictionary<string, string>
    {
        public override string ToString()
        {
            return $"{this["expression"]}: {this["error"]}";
        }
    }

    public static IEnumerable<object?[]> Expectations()
    {
        var folder = TestFolderHelper.TestFolder;
        var filePath = Path.Combine(folder, "errors.yml");
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
    public void ParsedExpression_ShouldThrow(Expectation expectation)
    {
        var expression = expectation["expression"];
        var error = expectation["error"];
        var parser = new TagExpressionParser();
        var ex = Assert.ThrowsException<TagExpressionException>(() => parser.Parse(expression));
        Assert.AreEqual(error, ex.Message);
    }
}
