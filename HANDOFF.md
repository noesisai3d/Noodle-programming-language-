# Handoff

**Session 1 · 2026-09-24**

## Where I stopped

G0 criteria are met. G1 is in progress: Seed Part A is implemented and tested (54 conformance programs + 7 examples, all green). Waiting on the owner for QUESTIONS.md, especially Q1–Q3 (bootstrap language, cap, SEED approval).

## Start the next session with

```text
cc -std=c99 -Wall -Wextra -pedantic -O2 -o build/noodle-seed bootstrap/seed.c
timeout 120 build/noodle-seed test conformance
timeout 120 build/noodle-seed test examples
```

Always use a timeout. The bootstrap has no step limit, so an endless loop hangs the run.

## Next smallest steps (in order)

1. **Seed B6 + B9 (files, folder listing, arguments), with a NEP.** Then write `noodle/tools/purity.noodle`, the first Noodle tool and the last G1 criterion. It needs: read a file, list a folder, count lines, and classify by file extension.
2. Golden tests for the untested bootstrap messages (spec/ERRORS.md, last paragraph).
3. A quick fuzz pass on the bootstrap's lexer and parser (random bytes → must print a friendly error, never crash).
4. Seed B1–B5 (records, tables, kinds, `choose`, `try`), each with a NEP, then B7, B10, and B11. These are what the compiler needs before G2.
5. If the owner agrees (Q17), add the CI config: build the bootstrap, then run both suites under a timeout.

## Surprises

- Writing expected output by hand before running found 4 design problems and 1 bad test on the first day (DECISIONS/0004). Keep doing it.
- The two-readings rule (`double 4 plus 1` is an error) is strict and will make the compiler's source a little wordier. That's deliberate (Q12).
- Using `pkill -f noodle-seed` from a shell whose own command line contains `noodle-seed` kills that shell. Use `timeout` instead.
