import assert from 'assert'
import fs from 'fs'
import yaml from 'js-yaml'

import parse from '../src/index.js'
import { testDataDir } from './testDataDir.js'

type Tests = {
  parsing: {
    expression: string
    formatted: string
  }[]
  errors: {
    expression: string
    error: string
  }[]
  evaluations: {
    expression: string
    tests: {
      variables: string[]
      result: boolean
    }[]
  }[]
}

const tests = yaml.load(fs.readFileSync(`${testDataDir}/tests.yml`, 'utf-8')) as Tests

describe('Tag Expressions', () => {
  describe('Parsing', () => {
    for (const test of tests.parsing) {
      it(`parses "${test.expression}" into "${test.formatted}"`, () => {
        const node = parse(test.expression)
        assert.strictEqual(node.toString(), test.formatted)

        const nodeFromFormatted = parse(node.toString())
        assert.strictEqual(nodeFromFormatted.toString(), test.formatted)
      })
    }
  })

  describe('Evaluation', () => {
    for (const evaluation of tests.evaluations) {
      describe(evaluation.expression, () => {
        for (const test of evaluation.tests) {
          it(`evaluates [${test.variables.join(', ')}] to ${test.result}`, () => {
            const node = parse(evaluation.expression)
            assert.strictEqual(node.evaluate(test.variables), test.result)
          })
        }
      })
    }
  })

  describe('Errors', () => {
    for (const test of tests.errors) {
      it(`fails to parse "${test.expression}" with "${test.error}"`, () => {
        assert.throws(() => parse(test.expression), { message: test.error })
      })
    }
  })
})
