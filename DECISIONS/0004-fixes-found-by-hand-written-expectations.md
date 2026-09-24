# 0004 — Parser changes found by hand-written expected files

**Status:** ACCEPTED · 2026-09-24

## Context
All expected outputs were written by hand from the draft spec before being compared with the bootstrap. The first runs disagreed in five places.

## What we found and decided
1. **`factorial n minus 1` silently meant `factorial(n) minus 1`.** That's a real ambiguity, so it's now an error that shows both readings (`errors/ambiguous_call`). It applies to teaching calls and to phrases like `length of`.
2. **`make a is 5` was refused** because `a` was reserved for `a list of`. That's hostile to beginners. `a` and `an` are now phrase words recognized by lookahead, like `length of`.
3. **`a list of 1, 2 is a list of 1, 2` swallowed the `is` into the last item.** List items are now parsed as values (arithmetic level), not as conditions.
4. **`say a new list` printed the literal words.** The bare-words rule now needs at least one word that is neither a name nor a Noodle word.
5. **A test had its own bug** (`repeat until count is 0` from 7 in steps of 3 never ends). Fixed the test. This exposed that the bootstrap has no step limit. That limit is also noted in STATUS.md.

## Consequences
Two of these were design decisions, not just bug fixes (1 and 2). They are open questions for the owner, and they are listed in BEGINNER_LOG.md for beginner testing.
