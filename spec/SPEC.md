# The Noodle Language Specification

**Version:** 0.1.0-draft · **Edition:** none yet (proposed first edition: `2027`)
**Status:** DRAFT. No separate specification was supplied with the mission, so this one is derived from `MISSION.md` (§8).
**Every syntax decision in this document is PROPOSED** and waits for the owner's approval (§74.1). Items marked **OPEN** are open questions. They are collected in [`QUESTIONS.md`](../QUESTIONS.md).

Where this document and the bootstrap disagree, this document wins and the bootstrap gets fixed. Where this document is silent, nothing is decided yet. The implementation MUST NOT invent an answer. It must stop with a friendly error instead (Prime Directive 5).

The subset the bootstrap runs is defined in [`SEED.md`](SEED.md). Sections marked **[Seed]** below are in Seed and have conformance tests under `conformance/seed/`.

---

## 1. Programs and lines

**[Seed] PROPOSED.**

- A program is a UTF-8 text file ending in `.noodle`. A leading byte-order mark is ignored.
- **One instruction per line.** A line ends at a line break (`\n` or `\r\n`).
- **Indentation has no meaning.** Blocks are closed by `end` (conformance: `if/no_indentation`). The formatter will indent them (§57).
- **Comments** start with `#` and run to the end of the line. A `#` inside quotes is text (conformance: `make/comments`). *PROPOSED, since the mission names comments as a core feature but not their syntax.*
- **Blank lines** are ignored.
- **Line continuation: OPEN.** There is none in Seed yet. The proposal is that long lines stay long, and the formatter never wraps them.

## 2. Words, names, and case

**[Seed] PROPOSED.**

- **Keywords are case-insensitive** (§9). `MAKE`, `Make`, and `make` are the same word.
- **Names are case-insensitive too.** `Score` and `score` are the same name (conformance: `make/case_insensitive`). Reason: two names that differ only by case are a classic beginner trap, and Noodle never guesses.
- **Names are single words**: letters, digits, and `_`, not starting with a digit. `high_score` is fine. `high score` is two words. *Reason: multi-word names make every line ambiguous with keywords.* **OPEN:** allow multi-word names later with a NEP.
- **Unicode names:** letters beyond ASCII are allowed in names. Normalization (NFC plus case folding) is PROPOSED for the full language. The bootstrap compares non-ASCII names byte by byte and lowercases ASCII only.
- **Reserved words** can't be names. Using one is error `reserved_name`. The Seed reserved list: `say ask make is if otherwise end repeat times while until for each in from to add teach do give back and or not plus minus divided by yes no the of stop skip than with contains`.
- **Phrase words** are ordinary names that start built-in phrases only in a fixed pattern: `a list of`, `a new list`, `length of`, `item … of`, `first item of`, `last item of`, `remainder of`, `whole part of`. So `make a is 5` and `for each item in shopping` are both fine.

## 3. Values

**[Seed] PROPOSED**, following §12.

| Kind | Written as | Shown by `say` as |
|---|---|---|
| number | `42`, `-20`, `0.1`, `2.50` | `42`, `-20`, `0.1`, `2.5` |
| text | `"hello"` | `hello` |
| yes/no | `yes`, `no` | `yes`, `no` |
| list | `a list of 1, 2, 3`, `a new list` | `1, 2, 3`, `an empty list` |

- **Numbers are exact rationals.** Whole numbers never overflow. `0.1 plus 0.2 is 0.3` is `yes`.
- **Showing numbers:** whole numbers in plain digits. Fractions whose denominator divides a power of ten are shown as decimals (`7 divided by 4` → `1.75`). Every other fraction is shown in lowest terms as `n/d` (`1 divided by 3` → `1/3`). *PROPOSED; OPEN: whether beginners prefer `0.333…`.*
- **Text** is a sequence of Unicode characters stored as UTF-8. Inside quotes, `""` stands for one quote mark. `[` and `]` start and end a slot (§6). `[[` and `]]` stand for literal brackets.
- **yes and no** are the only truth values. `if 0` is an error (`if_not_yes_no`).
- **Nothing:** a teaching that doesn't `give back` produces *nothing*. Using nothing as a value is an error that names the teaching (`nothing_given_back`). Programs can't write *nothing* directly. **OPEN:** whether a `nothing` literal ever exists.
- **Values never change; names do.** `add 3 to b` gives `b` a new list. Any other name holding the old list still sees the old list (conformance: `lists/values_never_change`).
- **Equality** (`is`) compares values of the same kind: numbers by value, text byte by byte (Unicode normalization is OPEN), lists item by item. Comparing different kinds is an error (`compare_kinds`), because `5 is "5"` is almost always a bug.

## 4. Statements

**[Seed] PROPOSED.** Every statement is one line, and a block is closed by `end`.

| Statement | Meaning |
|---|---|
| `say V` | Show V followed by a line break. See §4.1 for bare words. |
| `ask V` | Show V as a question, read one line of input, and put the reply (text) in the name `answer`. |
| `make N is V` | Give the name N the value V. It works both the first time and to change it. |
| `add V to N` | N must hold a list. N now holds a new list with V at the end. |
| `if C` … `otherwise if C` … `otherwise` … `end` | Choose one block. C must be yes/no. |
| `repeat V times` … `end` | V must be a whole number ≥ 0. |
| `repeat while C` … `end` / `repeat until C` … `end` | C is checked before each round. |
| `for each N in V` … `end` | V is a list (each item) or text (each character). The loop walks V as it was when the loop started (conformance: `loops/snapshot`). |
| `for each N from A to B` … `end` | Counts by 1 from whole number A to B, downward if A > B. This replaces the §13 candidate `count from 1 to 10`, so counting reuses `for each` instead of adding a keyword. |
| `stop` / `skip` | Leave the nearest loop, or go to its next round. Only inside loops. |
| `teach T P and Q` … `end` | Define the teaching T with inputs P, Q, and so on. Only at the top of a file. |
| `give back V` | End the teaching and produce V. |
| `do T args` | Use a teaching on its own line and ignore its result. |

### 4.1 Bare words after `say`

§9's first example is `say hello`, so bare words must work. **PROPOSED rule (one meaning per line):**

1. If every word after `say` is a plain word (not a name, not a Noodle word), the words themselves are shown, spaced as typed and with their spelling kept: `say Hello there` → `Hello there`.
2. If any plain word is one or two letters away from a name, it's an error (`bare_word_typo`), because `say mony` next to `money` is probably a typo.
3. If the words mix plain words and names (`say you have money`), it's an error (`mixed_words_names`) that suggests the quoted form with `[ ]`.
4. Otherwise the rest of the line is a value (an expression).

## 5. Expressions and precedence

**[Seed] PROPOSED.** From the loosest binding to the tightest:

| Level | Forms | Notes |
|---|---|---|
| 1 | `A and B`, `A or B` | A line may not mix `and` with `or` (`mixed_and_or`). **OPEN:** a clarifying form such as `both … and …` / `either … or …`. |
| 2 | `not A` | |
| 3 | `A is B`, `A is not B`, `A is bigger than B`, `A is smaller than B`, `A is at least B`, `A is at most B`, `A contains B` | Only one comparison per level: `a is b is c` is an error. |
| 4 | `A plus B`, `A minus B` | left to right |
| 5 | `A times B`, `A divided by B`, `remainder of A divided by B` | left to right |
| 6 | `length of V`, `item N of V`, `first item of V`, `last item of V`, `whole part of V`, `the number in V`, teaching calls `T a and b` | each takes one simple value |
| 7 | numbers, text, `yes`, `no`, names, `a list of …`, `a new list` | |

**The two meanings of `is`** (§10): in `make N is V`, the first `is` after the name binds. Everywhere else, `is` compares. `make same is a is b` therefore means "same becomes (a is b)". **OPEN:** whether to require a clearer form there.

**The two-readings rule** (§10): a phrase at level 6 takes one simple value. If `plus`, `minus`, `times`, or `divided by` follows that value, the line could mean two things (`factorial n minus 1`), so it is error `ambiguous_call`. The error shows both readings and suggests naming the value first. `remainder of A divided by B` is exempt because it needs the division.

**List items** are level-4 values: `a list of 1, 2 is a list of 1, 2` compares two lists. A list literal can't directly contain another list literal on the same line.

**Evaluation order:** left to right. `and` and `or` stop early: the right side isn't evaluated when the left side decides the answer.

## 6. Text slots

**[Seed] PROPOSED.** Inside quotes, `[V]` is replaced by V shown as `say` would show it: `"Hi [name], you have [money plus 5] coins"`. Slots may hold any expression. A quote mark inside a slot is written `""`.

## 7. Teachings and scope

**[Seed] PROPOSED.**

- Teachings may be used before the line that defines them. A project is one world of names (§14).
- A teaching sees its inputs, the names it makes, and the top-level names of the program (read-only).
- **Nothing shadows silently** (§14). An input or name inside a teaching that matches a top-level name is error `shadowing`. A teaching can't change a top-level name. It gives back a value instead.
- Inputs are passed as values (they never change, so there is no aliasing).
- **Recursion** is allowed up to a depth limit. The bootstrap uses 2,000. The World VM's default is an owner decision (§74.8). Going deeper is error `too_deep`.
- A teaching is used as a value (`say double 4`) or as a line of its own (`do greet "Mo"`). A line that starts with a teaching's name without `do` is error `teaching_without_do`.

## 8. Errors

Errors follow §15. Every error names the line, explains in plain words, shows the line as written, and suggests a fix. The error catalogue is [`spec/ERRORS.md`](ERRORS.md). **PROPOSED:** codes like `N0001` get assigned when the catalogue moves into Noodle data files (G3). Until then, errors are identified by their golden-test names.

## 9. World model (not yet specified)

The semantics of steps, transitions, effects, the journal, history, provenance, checkpoints, branches, experiments, rewind, replay, observers, and protections (§18–§33) will be written here before G3. Nothing in the bootstrap records history: the bootstrap is deliberately "dumb" (§5). The world model will be implemented in Noodle, by the World VM.

## 10. Planned for Noodle but outside Seed

Records, tables, kinds, `choose`, failure handling, file input and output, bytes, command-line arguments, and more text operations (all §13 Tier 1) are planned. SEED.md lists the ones the compiler needs and the syntax proposed for each.

## 11. Keyword budget

| Word(s) | Introduced by | Beginner evidence |
|---|---|---|
| `say ask make if otherwise end repeat for each add teach do give back and or not` | original core (§11) | pre-existing |
| `is plus minus times divided by yes no` | original core (§11) | pre-existing |
| `in from to times while until stop skip` | loops, PROPOSED (§13 #5) | heuristics only, none yet |
| `bigger smaller than at least most contains` | comparisons, PROPOSED | heuristics only, none yet |
| `the of with` | phrases, PROPOSED | heuristics only, none yet |

No NEPs exist yet, so none of the PROPOSED rows is approved. See [`BEGINNER_LOG.md`](../BEGINNER_LOG.md).

## Changelog

- **0.1.0-draft (2026-09-24):** first draft, derived from MISSION.md. Covers the Seed core. Everything is PROPOSED.
