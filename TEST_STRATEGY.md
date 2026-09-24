# Test strategy

## Today (G1)

| Suite | Where | What it proves | Count |
|---|---|---|---|
| Seed conformance | `conformance/seed/` | The meaning of every Seed construct, with exact output | 30 programs |
| Golden errors | `conformance/seed/errors/` | The exact text of every tested error, including "Did you mean?" | 24 programs |
| Examples | `examples/` | Every §60 example that Seed can express, with exact output | 7 programs |

Run them:

```text
cc -std=c99 -O2 -o build/noodle-seed bootstrap/seed.c
build/noodle-seed test conformance
build/noodle-seed test examples
```

**Rules we follow (§6):**

- Expected files are written by hand from the specification, never copied from the interpreter's output. This rule has already caught real bugs: see DECISIONS/0004.
- A missing `.expected` file fails, and there are no skips.
- Every fixed bug gets a test.

## Evidence of test strength

- **Mutation checks (by hand, this session):** 7 distinct deliberate faults were planted in a copy of the bootstrap: `>` became `>=`, `<=` became `<`, `and` became `or`, shared lists were changed in place by `add`, `stop` was ignored, decimal scaling was off by one, and the sign comparison was flipped. The first round let two mutants survive (`>` became `>=`, and the decimal scaling), which showed real gaps. We added `logic/boundaries` and `numbers/decimals`, and now all 7 are caught. Two of them (`stop` ignored, sign flipped) are caught only by the test run hanging, because the bootstrap has no step limit. A Noodle mutation tool comes later (§52).
- **Sanitizers:** the whole suite passes under AddressSanitizer and UndefinedBehaviorSanitizer.
- **Two compilers:** the suite passes when the bootstrap is built with gcc and with clang.

## Known gaps

- No fuzzing yet. First target: the Noodle lexer and parser at G2. A fuzz pass on the bootstrap's lexer is cheap and should happen in G1.
- These errors have no golden test yet: empty list, whole-number-needed, the ASCII limit, ran out of answers, nothing to read. They're listed in spec/ERRORS.md.
- No timeout inside test mode. A hanging program hangs the run.
- Coverage isn't measured. A Noodle coverage tool is planned (§52).

## Later (by gate)

G2: lexer and parser round trips, plus metamorphic tests (reformatting and comments don't change meaning). G3: World VM against the bootstrap as a differential test on all of `conformance/seed`, plus journal golden tests. G5–G6: property tests for Laws 1–8. G7: native code against the World VM (Law 9). Later: chaos tests, security tests, benchmarks, and documentation tests. All of these test runners will be written in Noodle.
