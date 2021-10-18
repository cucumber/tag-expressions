package tagexpressions

import (
	"fmt"
	"gopkg.in/yaml.v3"
	"io/ioutil"
	"testing"

	"github.com/stretchr/testify/require"
)

type ParsingTest struct {
	Expression string `yaml:"expression"`
	Formatted  string `yaml:"formatted"`
}

func TestParsing(t *testing.T) {
	contents, err := ioutil.ReadFile("../testdata/parsing.yml")
	require.NoError(t, err)
	var tests []ParsingTest
	err = yaml.Unmarshal(contents, &tests)
	require.NoError(t, err)

	for _, test := range tests {
		name := fmt.Sprintf("parses \"%s\" into \"%s\"", test.Expression, test.Formatted)
		t.Run(name, func(t *testing.T) {
			expression, err := Parse(test.Expression)
			require.NoError(t, err)

			require.Equal(t, test.Formatted, expression.ToString())

			expressionAgain, err := Parse(expression.ToString())
			require.NoError(t, err)

			require.Equal(t, test.Formatted, expressionAgain.ToString())
		})
	}
}
