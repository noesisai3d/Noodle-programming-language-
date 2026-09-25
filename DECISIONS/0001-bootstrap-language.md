# 0001 — Bootstrap language: C99 (reversible default)

**Status:** ACCEPTED AS A REVERSIBLE DEFAULT · owner decision pending (§74.2)
**Date:** 2026-09-24

## Context
Noodle needs a temporary interpreter for Noodle Seed (§5). It must be small, boring, widely available, and easy to audit, and it will be retired at G9. The owner hasn't chosen its language yet. §73 says to pick a reversible default and keep working.

## Options
1. **C99:** every host has a compiler. No runtime. It can be built from a hex seed via stage0 → M2-Planet → GNU Mes → TinyCC, so it fits G11.
2. **Go:** pleasant, but a large toolchain that is hard to reach from nothing.
3. **Python:** the mission forbids needing Python to build Noodle (§3).
4. **Scheme (for GNU Mes):** fits from-nothing well, but fewer people can audit it.
5. **Hand-written assembly:** the most auditable, but far too slow to develop.

## Decision
C99 in one file, `bootstrap/seed.c`, using the standard library plus POSIX `dirent.h` for test mode.

## Consequences
- We must implement exact bignum fractions in C. That is about 150 lines, and the same algorithm will later be written in Noodle.
- Test mode doesn't build with MSVC until the Noodle test runner exists, because test mode uses POSIX `dirent.h`.
- If the owner picks another language, the bootstrap is about 1,100 lines, and all tests are language-neutral `.noodle` + `.expected` files, so they carry over unchanged.
