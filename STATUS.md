# Status

**Last updated:** 2026-09-25 · **Stage:** early development · **Gate:** G1 (Seed) in progress, 4 of 5 criteria proven (the Seed subset spec still needs writing and owner approval); G0 partly done

This page lists what works today and the proof for each claim. If something isn't listed under "Works today," assume it doesn't exist yet.

## Works today

Everything below runs on the temporary bootstrap interpreter, [`bootstrap/seed.c`](bootstrap/seed.c) (C99, 1,246 lines).

| What | Proof |
|---|---|
| The bootstrap builds with no warnings | `cc -std=c99 -Wall -Wextra -pedantic -O2 -o build/noodle-seed bootstrap/seed.c`, with gcc 13.3 and clang |
| `say hello` runs | `examples/hello` |
| The core language: `say`, `ask`, `make`, text with `[slots]`, yes/no, lists, `if`/`otherwise`, `repeat`, `for each`, `stop`/`skip`, `teach`/`do`/`give back` | `build/noodle-seed test conformance` → `66 passed, 0 failed` |
| Exact numbers (`0.1 plus 0.2` → `0.3`, `1 divided by 3` → `1/3`, whole numbers of any size) | `conformance/seed/numbers/` |
| Friendly error messages, with their exact text checked | 29 tests in `conformance/seed/errors/` |
| Files: `the text of file`, `the lines of file`, `the files in folder`, `write … to file`; plus `the words of`, `the arguments`, `starts with`, `ends with` | `conformance/seed/files/`, `conformance/seed/text/words`, `conformance/seed/text/starts_ends` |
| The first tool written in Noodle: the purity audit | `build/noodle-seed run noodle/tools/purity.noodle` → `Purity Score: 10.6%`, `Result: PASS`. Every number it prints matches an independent recount with `awk` and `find`. A planted `.py` file outside `bootstrap/` turns the result to `FAIL`. |
| Example programs | `build/noodle-seed test examples` → `7 passed, 0 failed` |
| The bootstrap is memory-safe on the test suite | the whole suite passes when built with `-fsanitize=address,undefined` |
| The tests catch real bugs | all 11 bugs planted on purpose in a copy of the interpreter were caught |

## Not built yet

- **The language specification.** The first draft was removed and is being rewritten, so the syntax may change.
- **Records, tables, kinds, `choose`, `try`, bytes, and a way to set an exit code**, and more. The compiler needs all of these.
- **The Noodle compiler and the World VM.** Both will be written in Noodle, and neither exists yet.
- **Every world feature:** history, `why`, checkpoints, branches, rewind, replay, `watch`, `protect`, and `remember`.
- **Anything installable.** There are no releases and no packages.
- **CI.** There's no automated build yet.

## Known limits of the bootstrap

- It has no step limit, so an endless loop runs forever. Run the tests under a timeout.
- It counts letters (`length of`, `item N of`) only in plain English (ASCII) text. For other text it stops with a friendly error instead of giving a wrong answer.
- It reads and writes files without asking for permission. Permissions (grants) will come with the Noodle runtime. The bootstrap is only for running Noodle's own tools and tests.

## Known limits of the purity tool

- It prints `Result: FAIL` but still exits with code 0, because Seed has no way to set an exit code yet. CI can't rely on it until that exists.
- It has no automated test of its own yet. Its output changes whenever the repository changes, so for now it's checked by the independent recount above.

## Purity

The mission's goal is a Noodle built entirely by Noodle. Today the **Purity Score is 10.6%**, measured by the purity tool: 138 Noodle logic lines against 1,163 in the C bootstrap. The test programs and examples are also written in Noodle, but they aren't counted in the score.
