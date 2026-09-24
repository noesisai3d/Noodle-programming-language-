# The bootstrap

**Status:** exists, temporary, capped. 1,137 of 3,000 lines.

## What it is

`bootstrap/seed.c` is one C99 file that interprets **Noodle Seed** ([spec/SEED.md](spec/SEED.md)). It exists only because a new language can't run its first program by itself (§5). It is:

- **Tiny.** 3,000 lines is the hard cap, recorded in ARCHITECTURE.md.
- **Dumb.** A lexer, a recursive-descent parser, and a tree-walking interpreter. No optimizations, no world features, no tooling except the minimal test mode.
- **Isolated.** Nothing else in the repository includes or depends on its internals. Its only interface is the command line below.
- **Mortal.** It will be retired at G9 and kept frozen only for audits.

## Proposed language: C99. Owner decision pending (§74.2)

**Recommendation: C99, one file, standard library plus POSIX `dirent.h`.** This is a reversible default. See [DECISIONS/0001](DECISIONS/0001-bootstrap-language.md).

Why C99:

1. **Available everywhere.** Every Tier-1 host has a C compiler, or can get one (gcc, clang, MSVC, tcc).
2. **Boring and auditable.** No build system, no package manager, no runtime. One compile command.
3. **It fits the from-nothing chain (§5, G11).** The bootstrappable-builds chain (stage0 hex seed → M2-Planet → GNU Mes → TinyCC) ends in a C compiler, so a small C99 bootstrap can later be built with no pre-installed compiler at all. A Python, Go, or Rust bootstrap would put a big runtime in that chain.
4. **It makes us implement our own exact numbers,** which is the same work Noodle's runtime has to do anyway.

Alternatives considered: Go (large toolchain, harder from nothing), Python (the mission forbids needing Python to build Noodle), a hand-written assembly interpreter (auditable, but far too slow to develop in), and Scheme (a nice fit for from-nothing via GNU Mes, but less widely installed).

## Building and running it

The only build step, per §65:

```text
cc -std=c99 -O2 -o build/noodle-seed bootstrap/seed.c
```

Tested with gcc 13.3 and clang, with `-Wall -Wextra -pedantic` giving no warnings, and under AddressSanitizer and UndefinedBehaviorSanitizer with no errors (see STATUS.md).

```text
build/noodle-seed run program.noodle     run a Seed program
build/noodle-seed test folder            the minimal test mode
```

Exit codes: `0` success, `1` the program stopped with an error (or a test failed), `2` wrong usage, `3` out of memory.

## The minimal test mode

`noodle-seed test folder` finds every `*.noodle` file under the folder, in sorted order, runs it, and compares everything it printed (output and errors, in order) with `*.expected` byte for byte. A missing `.expected` file is a failure, never a skip. `*.input` supplies answers to `ask`, one per line, and each answer is echoed after its question so the transcript is readable. The real test runner will be written in Noodle (G4).

## What the bootstrap deliberately does not do

- It keeps no history, provenance, or journal. Those belong to the World VM, written in Noodle.
- It has no step limit. Only recursion depth is limited (2,000), so deep recursion gives a friendly error instead of a crash. **Known consequence:** an endless loop in a test hangs the test run. CI must run the tests under a timeout.
- It doesn't free memory after a program stops with an error. The process ends soon after, and in test mode the leak is bounded by the size of the suite.
- It doesn't normalize Unicode, and it counts characters only in ASCII text (the Seed ASCII limit).
