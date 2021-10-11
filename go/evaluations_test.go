package tagexpressions

import (
	"fmt"
	"gopkg.in/yaml.v3"
	"io/ioutil"
	"strings"
	"testing"

	"github.com/stretchr/testify/require"
)

type Evaluation struct {
	Expression string `yaml:"expression"`
	Tests      []Test `yaml:"tests"`
}

type Test struct {
	Variables []string `yaml:"variables"`
	Result    bool     `yaml:"result"`
}

func TestEvaluations(t *testing.T) {
	contents, err := ioutil.ReadFile("../testdata/evaluations.yml")
	require.NoError(t, err)
	var evaluations []Evaluation
	err = yaml.Unmarshal(contents, &evaluations)
	require.NoError(t, err)

	for _, evaluation := range evaluations {
		for _, test := range evaluation.Tests {
			variables := strings.Join(test.Variables, ", ")
			name := fmt.Sprintf("evaluates [%s] to %t", variables, test.Result)
			t.Run(name, func(t *testing.T) {
				expression, err := Parse(evaluation.Expression)
				require.NoError(t, err)

				result := expression.Evaluate(test.Variables)
				require.Equal(t, test.Result, result)
			})
		}
	}
}
