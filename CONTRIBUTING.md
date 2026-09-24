# Contributing to Noodle

Thank you for your interest! Noodle is at a very early stage, and that means some kinds of help matter a lot right now.

## The most useful help today

**1. Try it and tell us what confused you.** Noodle is meant to be readable by someone who has never programmed. If you build the bootstrap (see the [README](README.md)) and run the examples, anything that surprised you is valuable. That's especially true of an error message you didn't understand. Please open a [bug report](https://github.com/noesisai3d/Noodle-programming-language-/issues/new?template=bug_report.md). What beginners find confusing is recorded in [BEGINNER_LOG.md](BEGINNER_LOG.md) and shapes the language.

**2. Discuss syntax.** Every piece of Noodle's syntax is still a proposal. If you think a keyword is confusing, or you have a better idea, open a [syntax or feature proposal](https://github.com/noesisai3d/Noodle-programming-language-/issues/new?template=syntax_proposal.md). Before a new feature is accepted, it will need a written proposal (a *Noodle Enhancement Proposal*, or NEP) that answers the questions in [MISSION.md §17](MISSION.md): What problem does it solve? Can a beginner understand it? Can existing syntax already do it?

**3. Find ambiguity.** A core rule of Noodle is that every line has exactly one meaning. If you can write a line that could reasonably mean two different things and Noodle silently picks one, that's a bug. Please report it.

## Code contributions

**Large code contributions aren't being accepted yet.** The project owner approves the direction of the language and its design, and the architecture is still being set. Small fixes to the bootstrap, tests, or docs are welcome as pull requests. For anything bigger, please open an issue first to talk it over.

If you do send a change:

- **Tests first.** A test is a program, `name.noodle`, plus its exact expected output, `name.expected` (and optionally `name.input` for answers to `ask`). Write the expected output by hand from what the language *should* do. Don't copy it from what the interpreter prints.
- **Keep everything passing:**
  ```text
  mkdir -p build
  cc -std=c99 -Wall -Wextra -pedantic -O2 -o build/noodle-seed bootstrap/seed.c
  build/noodle-seed test conformance
  build/noodle-seed test examples
  ```
- **Small commits** that say what changed, why, and how it was tested.
- **Never claim more than the tests prove,** in code comments, docs, or pull requests.

## The purity rule

Noodle is meant to be built entirely in Noodle. The only non-Noodle code allowed is the temporary bootstrap in `bootstrap/`, which is capped in size and will be retired. Please don't add code in any other language, and don't add build scripts (shell, Make, Python, and so on). See [MISSION.md §4](MISSION.md).

## Code of Conduct

Everyone taking part in this project is expected to follow the [Code of Conduct](CODE_OF_CONDUCT.md).

## License

By contributing, you agree that your contributions are licensed under the [Apache License 2.0](LICENSE), the same license as the project.
