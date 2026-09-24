# Purity audit

**Audit #1 · 2026-09-24 · done by hand.** `noodle purity` doesn't exist yet. It becomes a Seed program in G1 and replaces this manual count.

## Purity Score: **0%**

> Purity Score = Noodle logic lines ÷ all logic lines needed to build, test, and release Noodle (§4).

| Logic that builds, tests, or runs Noodle | Language | Logic lines* |
|---|---|---|
| `bootstrap/seed.c`, the interpreter and test mode | C99 | 1,051 |
| Noodle compiler, World VM, runtime, tools (`noodle/`) | Noodle | **0**, none written yet |

\* Non-blank lines that aren't comment-only.

**Honest reading:** everything that runs today is the C bootstrap. The score is 0% by design at this stage. It starts rising at G1, when `noodle purity` becomes the first Noodle tool, and climbs steeply at G2 and G3, when the compiler and World VM are written in Noodle.

For information only, since these are not counted in the score: the Noodle test programs and examples add 299 lines of Noodle across 61 files (`conformance/`, `examples/`). They show that the tests are already pure Noodle programs with plain-text expected output.

## Bootstrap

- `bootstrap/seed.c`: **1,137 of 3,000 lines** (38% of the cap).

## Host shims

None.

## Machine Layer files

None.

## Other files

| File(s) | Class | Allowed by |
|---|---|---|
| `*.md` | prose | §4 prose and data |
| `LICENSE` | prose | §4 |
| `conformance/**/*.expected`, `*.input`, `examples/*.expected`, `*.input` | test data | §4 prose and data |
| `.gitignore` | version-control data (one line) | §64 |

**Foreign code outside `bootstrap/`:** none. Checked with `git ls-files`: every non-Markdown, non-data file is either `bootstrap/seed.c` or a `.noodle` program.

**CI config:** none yet. When added, it will build the bootstrap exactly as BOOTSTRAP.md says and then run the tests. Once `noodle ci` exists, it will contain only that command (§65).
