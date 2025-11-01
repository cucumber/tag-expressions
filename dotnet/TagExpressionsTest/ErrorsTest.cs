using QueryTest;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TagExpressions;

namespace TagExpressionsTest;

[TestClass]
public class ErrorsTest
{
    public static IEnumerable<object?[]> Expectations()
    {
        var folder = TestFolderHelper.TestFolder;
        var filePath = Path.Combine(folder, "errors.yml");
        var fileContent = File.ReadAllText(filePath);
        var deserializer = new YamlDotNet.Serialization.Deserializer();
        var items = deserializer.Deserialize<List<Dictionary<string, string>>>(fileContent);
        foreach (var item in items)
        {
            yield return new object?[] { item };
        }
    }

    [TestMethod]
    [DynamicData(nameof(Expectations), DynamicDataSourceType.Method)]
    public void ParsedExpression_ShouldThrow(Dictionary<string, string> expectation)
    {
        var expression = expectation["expression"];
        var error = expectation["error"];
        var parser = new TagExpressionParser();
        var ex = Assert.ThrowsException<Exception>(() => parser.Parse(expression));
        Assert.AreEqual(error, ex.Message);
    }

}
