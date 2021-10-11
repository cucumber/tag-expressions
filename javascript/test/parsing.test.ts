import assert from 'assert'
import fs from 'fs'
import yaml from 'js-yaml'

import parse from '../src/index.js'
import { testDataDir } from './testDataDir.js'

type ParsingTest = {
  expression: string
  formatted: string
}

const tests = yaml.load(fs.readFileSync(`${testDataDir}/parsing.yml`, 'utf-8')) as ParsingTest[]

describe('Parsing', () => {
  for (const test of tests) {
    it(`parses "${test.expression}" into "${test.formatted}"`, () => {
      const node = parse(test.expression)
      assert.strictEqual(node.toString(), test.formatted)

      const nodeFromFormatted = parse(node.toString())
      assert.strictEqual(nodeFromFormatted.toString(), test.formatted)
    })
  }
})
