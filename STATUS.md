# Status

**Date:** 2026-09-24 · **Ladder:** stage 0, external bootstrap (in progress; G1 is not passed yet) · **Gate:** G0 criteria met, G1 in progress

## Inventory at the start (§76.3)

The repository held only `README.md` (two lines) and `LICENSE` (Apache-2.0). There was no specification, no code, and nothing to run. Nothing needed to be kept as a reference or removed.

## What works, with evidence

All commands were run from the repository root on Linux x86-64 with gcc 13.3.

| Claim | Evidence |
|---|---|
| The bootstrap builds cleanly | `cc -std=c99 -Wall -Wextra -pedantic -O2 -o build/noodle-seed bootstrap/seed.c`: no warnings. Also no warnings with clang. |
| `say hello` runs end to end | `build/noodle-seed run examples/hello.noodle` prints `hello` (test `examples/hello`) |
| Seed Part A runs | `build/noodle-seed test conformance` → `54 passed, 0 failed` |
| Examples run | `build/noodle-seed test examples` → `7 passed, 0 failed` |
| Exact numbers | `numbers/exact` (`0.1 plus 0.2` → `0.3`, `1 divided by 3` → `1/3`), `numbers/big` (2^100), `teach/recursion` (25!) |
| Friendly errors, exact text | 24 golden tests in `conformance/seed/errors/` |
| Values never change | `lists/values_never_change`, `loops/snapshot` |
| Memory safety of the bootstrap | the whole suite passes under `-fsanitize=address,undefined` |
| Tests catch bugs | 7 of 7 hand-planted mutants are caught (TEST_STRATEGY.md) |
| Bootstrap under its cap | `wc -l bootstrap/seed.c` → 1,137 of 3,000 |

## Gate G0 — Orientation (§76): criteria met

1. MISSION.md saved.
2. `spec/SPEC.md` drafted, with everything PROPOSED.
3. Inventory recorded above.
4. ARCHITECTURE.md, BOOTSTRAP.md, BOOTSTRAP_DEPENDENCIES.md, ROADMAP.md, TEST_STRATEGY.md, PURITY.md, and STATUS.md written.
5. `spec/SEED.md` drafted.
6. QUESTIONS.md lists 17 questions, each with a recommendation.
7. Report given (HANDOFF.md and the session report).

## Gate G1 — Seed: 3 of 5 criteria proven

- [ ] SEED.md approved (waiting on the owner: Q3)
- [x] The bootstrap runs Seed programs, under its cap (Part A, evidence above)
- [x] The minimal test mode works
- [x] `say hello` runs
- [ ] The first `noodle purity`, as a Seed program (needs Seed B6: files and folder listing)

## Not working or not built

- Seed Part B: records, tables, kinds, `choose`, `try`, files, bytes, arguments, more text, many-file projects.
- Anything under `noodle/`: compiler, World VM, tools. None of it exists yet.
- Every world feature. The bootstrap deliberately has none (§5).
- No CI config yet (Q17).
- Error codes (`N0001`, …) and the marker under the problem spot.

## Purity

Purity Score **0%** · bootstrap 1,137 of 3,000 lines · shims 0. See PURITY.md.
