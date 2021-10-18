package tagexpressions

import (
	"fmt"
	"gopkg.in/yaml.v3"
	"io/ioutil"
	"testing"

	"github.com/stretchr/testify/require"
)

type ErrorTest struct {
	Expression string `yaml:"expression"`
	Error      string `yaml:"error"`
}

func TestErrors(t *testing.T) {
	contents, err := ioutil.ReadFile("../testdata/errors.yml")
	require.NoError(t, err)
	var tests []ErrorTest
	err = yaml.Unmarshal(contents, &tests)
	require.NoError(t, err)

	for _, test := range tests {
		name := fmt.Sprintf("fails to parse \"%s\" with \"%s\"", test.Expression, test.Error)
		t.Run(name, func(t *testing.T) {
			_, err := Parse(test.Expression)
			require.Error(t, err)
			require.Equal(t, test.Error, err.Error())
		})
	}
}
