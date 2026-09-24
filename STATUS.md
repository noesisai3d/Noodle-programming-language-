# Status

**Last updated:** 2026-09-24 · **Stage:** early development · **Gate:** G1 (Seed) in progress; G0 partly done

This page lists what works today and the proof for each claim. If something isn't listed under "Works today," assume it doesn't exist yet.

## Works today

Everything below runs on the temporary bootstrap interpreter, [`bootstrap/seed.c`](bootstrap/seed.c) (C99, 1,137 lines).

| What | Proof |
|---|---|
| The bootstrap builds with no warnings | `cc -std=c99 -Wall -Wextra -pedantic -O2 -o build/noodle-seed bootstrap/seed.c`, with gcc 13.3 and clang |
| `say hello` runs | `examples/hello` |
| The core language: `say`, `ask`, `make`, text with `[slots]`, yes/no, lists, `if`/`otherwise`, `repeat`, `for each`, `stop`/`skip`, `teach`/`do`/`give back` | `build/noodle-seed test conformance` → `54 passed, 0 failed` |
| Exact numbers (`0.1 plus 0.2` → `0.3`, `1 divided by 3` → `1/3`, whole numbers of any size) | `conformance/seed/numbers/` |
| Friendly error messages, with their exact text checked | 24 tests in `conformance/seed/errors/` |
| Example programs | `build/noodle-seed test examples` → `7 passed, 0 failed` |
| The bootstrap is memory-safe on the test suite | the whole suite passes when built with `-fsanitize=address,undefined` |
| The tests catch real bugs | all 7 bugs planted on purpose in a copy of the interpreter were caught |

## Not built yet

- **The language specification.** The first draft was removed and is being rewritten, so the syntax may change.
- **Records, tables, kinds, `choose`, `try`, files, bytes**, and more. The compiler needs all of these.
- **The Noodle compiler and the World VM.** Both will be written in Noodle, and neither exists yet.
- **Every world feature:** history, `why`, checkpoints, branches, rewind, replay, `watch`, `protect`, and `remember`.
- **Anything installable.** There are no releases and no packages.
- **CI.** There's no automated build yet.

## Known limits of the bootstrap

- It has no step limit, so an endless loop runs forever. Run the tests under a timeout.
- It counts letters (`length of`, `item N of`) only in plain English (ASCII) text. For other text it stops with a friendly error instead of giving a wrong answer.

## Purity

The mission's goal is a Noodle built entirely by Noodle. Today, **0%** of the working code is Noodle: everything that runs is the temporary C bootstrap. The test programs and examples are already written in Noodle.
