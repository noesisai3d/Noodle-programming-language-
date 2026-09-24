# Noodle

A beginner-friendly programming language that remembers everything, so you can rewind, replay, and ask why.

```noodle
say hello

make money is 100

if money is bigger than 50
    say "You have enough"
otherwise
    say "You need more"
end
```

## Where Noodle is today

**Early. Honest numbers:**

| | |
|---|---|
| Ladder stage | **0, external bootstrap** (in progress) |
| Gate | G0 criteria met · G1 in progress (3 of 5 criteria) |
| Purity Score | **0%**: everything that runs today is the temporary C bootstrap ([PURITY.md](PURITY.md)) |
| Bootstrap | 1,137 of 3,000 lines |
| Tests | 54 conformance programs + 7 examples, all passing ([STATUS.md](STATUS.md)) |

What runs now is **Noodle Seed**, the core subset: `say`, `ask`, `make`, exact numbers, text with `[slots]`, yes/no, lists, `if`, loops, and `teach`, all with friendly errors. The world features (history, `why`, checkpoints, rewind, replay) are the heart of Noodle and **do not exist yet**. They'll be written in Noodle itself. See [ROADMAP.md](ROADMAP.md).

## Try it

You need a C compiler, but only for the temporary bootstrap:

```text
cc -std=c99 -O2 -o build/noodle-seed bootstrap/seed.c
build/noodle-seed run examples/money.noodle
build/noodle-seed test conformance
```

## The promise

A program that runs today keeps running.

## Read more

[MISSION.md](MISSION.md) · [spec/SPEC.md](spec/SPEC.md) (draft) · [spec/SEED.md](spec/SEED.md) · [ARCHITECTURE.md](ARCHITECTURE.md) · [BOOTSTRAP.md](BOOTSTRAP.md) · [QUESTIONS.md](QUESTIONS.md)

License: Apache-2.0.
