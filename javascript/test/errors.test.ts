import assert from 'assert'
import fs from 'fs'
import yaml from 'js-yaml'

import parse from '../src/index.js'
import { testDataDir } from './testDataDir.js'

type ErrorTest = {
  expression: string
  error: string
}

const tests = yaml.load(fs.readFileSync(`${testDataDir}/errors.yml`, 'utf-8')) as ErrorTest[]

describe('Errors', () => {
  for (const test of tests) {
    it(`fails to parse "${test.expression}" with "${test.error}"`, () => {
      assert.throws(() => parse(test.expression), { message: test.error })
    })
  }
})
