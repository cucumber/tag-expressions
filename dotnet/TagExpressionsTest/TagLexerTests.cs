using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using TagExpressions;

namespace TagExpressionsTest
{
    [TestClass]
    public class TagLexerTests
    {
        [TestMethod]
        public void TokenizesOperators_And()
        {
            var lexer = new TagLexer("AND");
            var token = lexer.NextToken();
            Assert.AreEqual(TagTokenType.And, token.Type);
            Assert.AreEqual(TagTokenType.End, lexer.NextToken().Type);
        }

        [TestMethod]
        public void TokenizesOperators_Or()
        {
            var lexer = new TagLexer("OR");
            var token = lexer.NextToken();
            Assert.AreEqual(TagTokenType.Or, token.Type);
            Assert.AreEqual(TagTokenType.End, lexer.NextToken().Type);
        }

        [TestMethod]
        public void TokenizesOperators_Not()
        {
            var lexer = new TagLexer("NOT");
            var token = lexer.NextToken();
            Assert.AreEqual(TagTokenType.Not, token.Type);
            Assert.AreEqual(TagTokenType.End, lexer.NextToken().Type);
        }

        [TestMethod]
        public void TokenizesParentheses()
        {
            var lexer = new TagLexer("( )");
            Assert.AreEqual(TagTokenType.LParen, lexer.NextToken().Type);
            Assert.AreEqual(TagTokenType.RParen, lexer.NextToken().Type);
            Assert.AreEqual(TagTokenType.End, lexer.NextToken().Type);
        }

        [TestMethod]
        public void TokenizesIdentifier()
        {
            var lexer = new TagLexer("foo");
            var token = lexer.NextToken();
            Assert.AreEqual(TagTokenType.Identifier, token.Type);
            Assert.AreEqual("foo", token.Value);
            Assert.AreEqual(TagTokenType.End, lexer.NextToken().Type);
        }

        [TestMethod]
        public void TokenizesEscapedCharacters()
        {
            var lexer = new TagLexer("foo\\ bar");
            var token = lexer.NextToken();
            Assert.AreEqual(TagTokenType.Identifier, token.Type);
            Assert.AreEqual("foo bar", token.Value);
            Assert.AreEqual(TagTokenType.End, lexer.NextToken().Type);
        }

        [TestMethod]
        public void HandlesEndOfInput()
        {
            var lexer = new TagLexer("");
            var token = lexer.NextToken();
            Assert.AreEqual(TagTokenType.End, token.Type);
        }

        [TestMethod]
        public void HandlesNullInput()
        {
            var lexer = new TagLexer(null);
            var token = lexer.NextToken();
            Assert.AreEqual(TagTokenType.End, token.Type);
        }

        [TestMethod]
        public void ThrowsOnIllegalEscape()
        {
            var lexer = new TagLexer("foo\\x");
            try
            {
                lexer.NextToken();
                Assert.Fail("Expected exception for illegal escape");
            }
            catch (Exception ex)
            {
                StringAssert.Contains(ex.Message, "Illegal escape");
            }
        }

        [TestMethod]
        public void PeekTokenDoesNotAdvancePosition()
        {
            var lexer = new TagLexer("foo bar");
            var peeked = lexer.PeekToken();
            var next = lexer.NextToken();
            Assert.AreEqual(peeked.Type, next.Type);
            Assert.AreEqual(peeked.Value, next.Value);
        }
    }
}
