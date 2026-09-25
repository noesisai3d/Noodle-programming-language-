# Noodle

**A programming language you can read like English, and one that remembers everything, so you can rewind, replay, and ask why.**

[![License: Apache 2.0](https://img.shields.io/badge/license-Apache%202.0-blue.svg)](LICENSE)
[![Status: early development](https://img.shields.io/badge/status-early%20development-orange.svg)](STATUS.md)

Noodle is a new programming language for people who are just starting out, and for anyone who has ever stared at a wrong number and wondered *how did that happen?* You write it in plain English words, with no braces or semicolons. The big idea is what comes next: every Noodle program will run inside a *world* that records what happened. You will be able to ask `why is balance -20`, rewind to before the mistake, fix it, and replay the same run to see the difference. And Noodle's own compiler will be written in Noodle.

That's the destination. The project is at the very beginning of the road, and this page says exactly how far along it is.

## A taste of Noodle

This program runs today:

```noodle
say hello

make money is 100

if money is bigger than 50
    say "You have enough"
otherwise
    say "You need more"
end
```

```text
hello
You have enough
```

When something goes wrong, Noodle explains it in plain words instead of showing a scary crash:

```noodle
make money is 100
say mony
```

```text
Line 2:
"mony" looks like the name money, so I'm not sure what you meant.

You wrote:
say mony

Did you mean money? If you meant the word itself, put it in quotes:
say "mony"
```

Numbers are exact, so `0.1 plus 0.2` is `0.3`, and `1 divided by 3` is `1/3`.

## Current status

**Early development. The core of the language runs, but the "world" features don't exist yet.**

What works today is a small, temporary interpreter called the *bootstrap* (one C file, [`bootstrap/seed.c`](bootstrap/seed.c)). It runs the core of the language: `say`, `ask`, `make`, exact numbers, text, lists, `if`/`otherwise`, loops, `teach` (reusable steps), and reading and writing files, with friendly error messages. 66 conformance tests and 7 examples pass.

The first tool written in Noodle itself also works: [`noodle/tools/purity.noodle`](noodle/tools/purity.noodle) audits the repository and reports how much of it is Noodle. Today's **Purity Score is 10.6%**. The rest is the temporary bootstrap, which is meant to shrink to nothing.

What doesn't exist yet: the real Noodle compiler (which will be written in Noodle), history, `why`, checkpoints, rewind, replay, and anything installable. The first draft of the language specification was removed and is being rewritten, so the syntax shown here may still change.

See **[STATUS.md](STATUS.md)** for exactly what works and the proof for each claim.

## How to follow along

There's nothing to install yet. To follow the project:

- **Watch** or **star** this repository to see progress.
- Read **[STATUS.md](STATUS.md)** for what works right now.
- Read **[MISSION.md](MISSION.md)** for the full plan.

If you have a C compiler and want to try the core language anyway, you can build the bootstrap from source:

```text
git clone https://github.com/noesisai3d/Noodle-programming-language-.git
cd Noodle-programming-language-
mkdir -p build
cc -std=c99 -O2 -o build/noodle-seed bootstrap/seed.c
build/noodle-seed run examples/money.noodle
build/noodle-seed test conformance
```

The bootstrap is temporary scaffolding, not the finished tool. It will be retired once Noodle can build itself.

## Why Noodle exists

Most languages make beginners learn punctuation before they learn ideas. When something goes wrong, they leave you guessing. Noodle aims to be easy to read *and* easy to write, with a debugger built into the language itself: a program remembers how every value came to be, so "why?" gets a real answer from what actually happened, not a guess.

## Roadmap

The full plan has twelve gates. Each one counts as passed only when tests prove it. Details are in [MISSION.md §72](MISSION.md).

| Gate | Goal | Status |
|---|---|---|
| G0 | Orientation: mission, spec draft, plan | 🟡 partly done (the spec draft is being rewritten) |
| G1 | **Seed:** a tiny bootstrap runs the core language | 🟡 **in progress**: core runs, and the first Noodle tool works; the subset spec still needs writing and approval |
| G2 | Noodle reads Noodle: a parser written in Noodle | ⬜ not started |
| G3 | Noodle runs Noodle: the World VM, with every run recorded | ⬜ not started |
| G4 | Real programs: records, tables, files, tests in plain English | ⬜ not started |
| G5 | A world that remembers: history, `why`, `watch`, `protect` | ⬜ not started |
| G6 | A world that travels: checkpoints, branches, rewind, replay | ⬜ not started |
| G7 | Native: Noodle's own compiler makes real executables | ⬜ not started |
| G8 | **Self-hosting achieved:** Noodle builds Noodle, bit for bit | ⬜ not started |
| G9 | The bootstrap is retired | ⬜ not started |
| G10 | Ecosystem: playground, editor support, more platforms | ⬜ not started |
| G11 | Built from nothing: no other language anywhere in the chain | ⬜ not started |

## What's in this repository

```text
MISSION.md        the full engineering plan
STATUS.md         what works today, with evidence
bootstrap/        the temporary C interpreter (retired later)
noodle/           Noodle's own source, written in Noodle (so far: the purity tool)
conformance/      test programs that define how Noodle behaves
examples/         small example programs, each with its expected output
DECISIONS/        records of design decisions and why they were made
BEGINNER_LOG.md   notes on what confuses beginners, and what changed because of it
```

## Contributing

Noodle is young, and the most valuable help right now isn't code. It's trying the examples, reporting confusing error messages, and discussing syntax. See **[CONTRIBUTING.md](CONTRIBUTING.md)**. Everyone taking part is expected to follow the [Code of Conduct](CODE_OF_CONDUCT.md).

## License

Apache License 2.0. See [LICENSE](LICENSE).
