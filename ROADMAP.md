# Roadmap

Organized by the gates of §72. Gates are passed in order, and a gate counts as passed only when STATUS.md records evidence for every criterion. Estimates are honest guesses for one focused agent working in sessions. They are not promises.

## G0 — Orientation · **criteria met, see STATUS.md**
MISSION.md saved. SPEC and SEED drafts written. Repository inventoried. Root documents, the first purity audit, and the owner questions written.

## G1 — Seed · in progress · Ladder 0
- [ ] `spec/SEED.md` approved by the owner.
- [x] The bootstrap, under its cap, runs Seed programs (Part A of SEED.md).
- [x] The minimal test mode works.
- [x] `say hello` runs (`examples/hello`).
- [ ] The first `noodle purity`, itself a Seed program, reports honestly. *Needs Seed B6 (files) and a folder listing.*
- Also before G2: Seed Part B (records, tables, kinds, `choose`, `try`, files, bytes, arguments, more text, many-file projects), each with a NEP, tests, and friendly errors.
- **Estimate:** 2–4 more sessions.

## G2 — Noodle reads Noodle · Ladder 1
The lexer, lossless syntax tree, and parser in `noodle/compiler/`, written in Seed and running on the bootstrap. They handle every example and their own source, with round-trip tests and friendly syntax errors from the catalogue. **Estimate:** several sessions.

## G3 — Noodle runs Noodle · Ladder 2
The World VM in `noodle/world/` as an explicit-state machine (§20), plus the effect gateway (§22), a journal for every run, golden error tests, and basic limits (§42). World-model spec text lands in SPEC.md §9 first. **Estimate:** weeks.

## G4 — Real programs
The rest of Tier 1 and Tier 3 #22–#25, files with grants, the Noodle test runner (which replaces the bootstrap test mode), and the gap analysis closed.

## G5 — A world that remembers
Transitions, history, provenance, `why`, `show`, `remember`, `watch`, `whenever`, and protections. Laws 1, 7, and 8.

## G6 — A world that travels
Checkpoints, branches, merge, experiments, `what if`, rewind, replay, replay with fixed code, and save and resume. Laws 1–8. North Star steps 1–7 on the World VM.

## G7 — Native · Ladder 3
Code generator, assembler, and linker for Linux x86-64 static ELF with direct system calls. Law 9. Reproducible output.

## G8 — SELF HOSTING ACHIEVED · Ladder 4
§49 steps 1–7 and Law 10.

## G9 — Bootstrap retired · Ladders 5–6
## G10 — Ecosystem
## G11 — From nothing · Ladder 7

The whole path to G8 is a many-month project. Anyone who says otherwise hasn't read §49.
