# 0002 — Seed syntax defaults

**Status:** PROPOSED · owner approval needed (QUESTIONS.md Q6–Q13, §74.1)
**Date:** 2026-09-24

## Context
No specification was supplied (§8). The core features (§11) are named, but most of their syntax isn't. The bootstrap needs concrete syntax to run anything, so these choices are the smallest ones that fit §9 and §10.

## Decisions (all PROPOSED)
| Topic | Choice | Main reason |
|---|---|---|
| Comments | `#` to the end of the line | short, familiar |
| `ask` | reply goes into `answer` | matches §13 #8; Scratch precedent |
| Counting | `for each n from 1 to 10` (counts down if the start is bigger) | no new keyword; names the counter |
| Teaching inputs | `teach area width and height`; used as `area 3 and 4` | commas stay for lists |
| Names | single words, case-insensitive | avoids keyword clashes and case bugs |
| Bare words after `say` | shown as words only when none is a name; a near-miss of a name, or a mix of words and names, is an error | §9 `say hello` works without guessing |
| Fractions | exact; decimal if it ends, otherwise `n/d` | exact and honest |
| Different kinds with `is` | error | catches `5 is "5"` bugs |
| Two readings (`double 4 plus 1`) | error; name the value first | §10 never guess |
| `and` mixed with `or` | error; split the line | §10 |
| Shadowing | teaching names can't match top-level names; teachings can't change top-level names | §14 nothing shadows silently |

## Consequences
Everything above has conformance tests, so any change the owner asks for shows up as specific failing tests, and it's clear what changed.
