# Architecture

**Status:** plan plus the first piece. Only the bootstrap exists today. Everything under `noodle/` is planned. Nothing here claims more than `STATUS.md` proves.

## The bootstrap size cap

**Hard cap: 3,000 physical lines, counting every file in `bootstrap/`.** This is the §5 target. It is PROPOSED to the owner (see QUESTIONS.md). `noodle purity` will enforce it in CI once it exists (G1). Until then it is checked by hand in each session report.
**Today:** 1,137 of 3,000 lines (`wc -l bootstrap/seed.c`).

## The pipeline (target, §34)

```text
source files (.noodle)
   │
   ▼
noodle/compiler/lexer      → tokens that keep every position
noodle/compiler/parser     → a lossless syntax tree (comments, spacing, positions)
noodle/compiler/names      → one world of names per project (§14)
noodle/compiler/checks     → kind inference and meaning checks
noodle/compiler/ir         → Noodle IR, with a documented text form (docs/IR.md)
   │
   ├──► noodle/world/vm    → the World VM, the reference engine (history, provenance, …)
   └──► noodle/backend     → code generator → assembler → linker → ELF, Mach-O, PE, Wasm
```

Every box is Noodle source. Before G7, the boxes run **on the bootstrap**: the bootstrap interprets the compiler's Seed source, and the compiler's output (IR) is run by the World VM, which is also Seed source running on the bootstrap. That is slow on purpose and correct first (§45).

## How the stages stack

```text
G1  bootstrap/seed.c ──runs──► any Seed program            (today: core features)
G2  bootstrap ──runs──► noodle/compiler (Seed) ──parses──► any Noodle program
G3  bootstrap ──runs──► compiler + noodle/world/vm ──runs──► Noodle programs, with a journal
G7  bootstrap ──runs──► compiler + backend ──emits──► native noodle (compiler₁)
G8  compiler₁ → compiler₂ → compiler₃, and compiler₂ ≡ compiler₃
```

## Key design commitments

1. **Reified execution (§20).** The World VM is written as an explicit-state machine: the position, the teaching frames, and the loop counters are ordinary Noodle records. It never uses the host's call stack for Noodle calls. This is why rewind, save-and-resume, and replay can be honest. Study list: CEK machines, defunctionalized interpreters.
2. **One effect gateway (§22).** Every effect the World VM performs (`say`, `ask`, files) goes through one teaching that checks grants and writes the journal.
3. **Immutable values with reference counting and reuse (§37).** The bootstrap already uses this: `add` changes a list in place only when nothing else holds it (see `N_ADD` in `bootstrap/seed.c`, and the conformance test `lists/values_never_change`). The World VM and native runtime will follow the same model, and a decision record will be backed by benchmarks.
4. **Exact numbers everywhere (§12).** The bootstrap implements arbitrary-size fractions. Noodle's runtime will implement its own, so results are identical on every platform.
5. **Errors are data (§15).** Today the messages are in the bootstrap. At G3 the catalogue moves into Noodle data files shared by every tool. The golden tests stay the same.

## Repository layout

Follows §62. Only these exist today:

```text
MISSION.md  README.md  LICENSE  + the root documents
bootstrap/seed.c           the temporary interpreter (C99)
spec/                      SPEC.md, SEED.md, ERRORS.md
conformance/seed/          the executable definition of Seed
examples/                  §60 examples, each with a .expected file
DECISIONS/                 decision records
build/                     local build output (ignored by git)
```

## Test file convention

`name.noodle` is the program. `name.expected` is its exact output, including error text. `name.input` is optional and holds one answer per line for `ask`. Documented in TEST_STRATEGY.md and DECISIONS/0003.
