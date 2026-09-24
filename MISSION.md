# NOODLE — ULTIMATE SELF-HOSTING LANGUAGE ENGINEERING MISSION
## Version 2 · The Pure Noodle Edition

> **Owner's note:** Give this entire document to your coding agent at the start of the project. The agent saves it in the repository as `MISSION.md` and re-reads Part I at the start of every session.

### How to read this document

**Priority order** when anything conflicts:

1. **Part I** — identity and non-negotiables. Never violated.
2. **The Noodle Language Specification** — the source of truth for syntax and meaning, once it exists.
3. **The rest of this document.**
4. **Your engineering judgment.**

If a conflict can't be resolved, stop and ask the owner (§74).

- **MUST, MUST NOT, SHOULD, and MAY** are used as in RFC 2119.
- **Candidate syntax** in this document is illustrative, not final. Final syntax is decided through the NEP process (§17), checked with beginners (§16), and recorded in the specification. Never treat a candidate keyword as final just because it appears here.
- **Nothing in this document is "done" until tests prove it.**

**Map of this document**

| Part | Sections | Covers |
|---|---|---|
| I | §0–§7 | Identity, Prime Directives, the Purity Doctrine, the bootstrap, honesty, the North Star |
| II | §8–§17 | The language: specification, principles, ambiguity, values, new features, projects, errors, evolution |
| III | §18–§33 | The world model and the ten World Laws |
| IV | §34–§47 | Implementation: architecture, Machine Layer, backend, memory, storage, security, platforms |
| V | §48–§51 | Self-hosting: the plan, the proof, the Purity Ladder, reproducible builds |
| VI | §52–§54 | Quality: testing, conformance, the red team |
| VII | §55–§61 | Tooling: command line, REPL, editor support, playground, recipes, examples, docs |
| VIII | §62–§70 | Discipline: repository, versions, Git, CI, releases, research, originality, governance, accessibility |
| IX | §71–§78 | Execution: the vertical method, gates, agent protocol, owner decisions, definition of done, first task |

---

## PART I — IDENTITY AND NON-NEGOTIABLES

### §0 — Your role

You are the principal language architect, compiler engineer, runtime engineer, systems engineer, security engineer, testing engineer, developer-experience designer, technical writer, release engineer, and open-source maintainer responsible for turning NOODLE into a **real, runnable, self-hosting programming language**.

This is not a mockup. Not a toy. Not a syntax wrapper around Python. Not "Python with English keywords." Not a transpiler. Not a JavaScript frontend. Not a C++ runtime with Noodle syntax painted on top.

**NOODLE MUST BECOME ITS OWN PROGRAMMING LANGUAGE AND ITS OWN SELF-HOSTING SOFTWARE ECOSYSTEM.**

Your job is to **build it** — and to **prove** every claim you make about it.

### §1 — The vision in one breath

Write code like plain English. Run it inside a *world* that remembers everything that happened. Ask *why* any value is what it is. Try *what if* without breaking anything. Checkpoint, branch, experiment, merge, rewind, and replay — for real. Save a running world and resume it tomorrow. And the entire system that does all of this — compiler, runtime, world engine, tools — is written in Noodle and built by Noodle.

### §2 — The Prime Directives

1. **NOODLE IS THE PRODUCT.** The compiler, runtime, world engine, standard library, tools, tests, and build system are written in Noodle.
2. **PURITY.** No other programming, scripting, or build language may exist in the product. The only exceptions are listed in §4 — counted, capped, and shrinking.
3. **NOODLE BUILDS NOODLE.** Self-hosting is proven by a bit-for-bit fixpoint (§49), never merely claimed.
4. **SIMPLE SURFACE, POWERFUL ENGINE.** A beginner reads Noodle like English. The power lives in the execution model, not in punctuation or piles of features.
5. **NEVER GUESS.** When code is ambiguous, Noodle stops with a friendly error. It never silently picks a meaning (§10).
6. **NEVER LIE.** History, provenance, rewind, and replay are real runtime data. If something can't be undone or replayed, Noodle says so plainly.
7. **NEVER SCARE.** No stack traces, no jargon, no raw crashes on screen. Every error has a line, a plain explanation, the source, and a suggested fix.
8. **SAFE BY DEFAULT.** Programs get no files, network, processes, or devices unless explicitly granted.
9. **DETERMINISTIC BY DEFAULT.** Same program + same recorded inputs = same world, on every supported machine.
10. **EVIDENCE OVER CLAIMS.** Nothing is "working," "done," "fast," "secure," or "self-hosted" without a test, command output, or measurement that proves it.
11. **SMALL STEPS, ALWAYS GREEN.** Build vertically, keep tests passing, commit often, document as you go.
12. **PRESERVE NOODLE'S IDENTITY.** Never turn Noodle into Python + Rust + Java + C++ + JavaScript. A feature enters only when it earns its place (§17).

### §3 — What Noodle must never become

- A compiler, runtime, or tool written in Python, C, C++, Rust, Go, Java, JavaScript, TypeScript, Zig, OCaml, Haskell, Lisp, Lua, or any other language. The temporary bootstrap (§5) is the only exception.
- A transpiler to another language. Noodle compiles to machine code and WebAssembly through its **own** backend. A WebAssembly binary is a machine target emitted directly — never produced through another language's toolchain.
- A front end for LLVM, GCC, QBE, Cranelift, or any external code generator.
- Anything that needs an external assembler or linker (`as`, `ld`, `lld`, `link.exe`).
- Anything that links libc or a foreign runtime library, except where the operating system makes that the only legal door (§4).
- A project that needs Python, Node.js, a JVM, .NET, Ruby, or Perl installed to build, test, or release.
- A project driven by shell scripts, Makefiles, CMake, Bazel, npm scripts, or Python build scripts.
- A language whose standard library wraps another language's library.
- A language whose `why` answers, explanations, or error messages come from an AI model's guesses instead of recorded runtime data. AI helpers may someday exist as tools *around* Noodle — never inside its semantics.

### §4 — The Purity Doctrine

"Purely Noodle" has a precise, testable meaning:

> **Every piece of logic in the product is Noodle source, compiled by Noodle's own compiler into machine code by Noodle's own backend.**

The complete list of allowed non-Noodle things:

| Allowed | Why it's unavoidable | Rule |
|---|---|---|
| The machine | Code must end as CPU instructions | Only machine code emitted by Noodle's own backend |
| The OS door | Programs must talk to the operating system | Linux: direct system calls. macOS: libSystem. Windows: the system DLLs (kernel32). WebAssembly: the host's imports. Called only from Noodle-emitted code — no C source anywhere |
| The bootstrap | A language can't run its first program by itself | Isolated in `bootstrap/`, size-capped, retired after self-hosting (§5) |
| Host shims | Browsers load WebAssembly through a loader; editors need a tiny client | Tiny, logic-free, listed in `PURITY.md`, never needed to build Noodle |
| Prose and data | Docs, examples, test fixtures | Markdown docs are fine. Every **config** file — project, lock, build, release — is Noodle syntax, not JSON, YAML, or TOML |
| CI host config | Hosted CI requires its own config file | It contains one command: `noodle ci`. All logic lives in Noodle |

Everything else **MUST** be Noodle.

**Purity is enforced, not hoped for.** Build `noodle purity` (in Noodle). It scans the repository, classifies every file, and reports:

- the **Purity Score** — Noodle logic lines ÷ all logic lines needed to build, test, and release Noodle (docs and data excluded; a frozen bootstrap kept only for audits is reported separately);
- the bootstrap's size against its cap;
- every host shim, with size and reason;
- every Machine Layer file (§35).

CI fails if foreign code appears outside `bootstrap/` or `shims/`, if the bootstrap exceeds its cap, or if shims grow without an approved decision record. The Purity Score appears in the README and in every release note.

**The purity dividend** — why this is worth the effort:

- Noodle implements its own numbers, text, and math, so results are identical on every platform — exactly what cross-machine replay needs.
- No foreign runtime means one self-contained executable with nothing else to install.
- The entire system is readable in one language, so the implementation itself teaches Noodle.

### §5 — The bootstrap

A language can't execute its first program without some initial execution mechanism, so a temporary bootstrap is permitted — **only** to reach self-hosting. It MUST be:

- **Tiny.** A hard size cap recorded in `ARCHITECTURE.md` (target: 3,000 lines or fewer), enforced by `noodle purity` in CI.
- **Dumb.** It interprets **Noodle Seed** only: the smallest strict subset of Noodle in which the compiler and the World VM can be written comfortably (likely the core features of §11 plus records, tables, kinds, `choose`, recursion, failure handling, and file input and output). Seed is defined in `spec/SEED.md`. Every Seed program is valid Noodle with identical behavior. The bootstrap never gets optimizations, tooling, or new features. If something can live in Noodle source, it goes there.
- **Isolated.** It lives only in `bootstrap/`. Nothing else depends on its internals.
- **Honest.** Documented in `BOOTSTRAP.md` and `BOOTSTRAP_DEPENDENCIES.md`, which lists every external dependency: why it exists, whether it's temporary, how it will be removed, and its current status. The dependency count only ever goes down.
- **Mortal.** Retired from normal development after SELF HOSTING ACHIEVED (§49); kept frozen only for audits until the from-nothing chain replaces it.

Rules:

- **Everything real is written in Noodle from the start.** The front end, the World VM, and every world feature are Noodle programs that *run on* the bootstrap during early development. The bootstrap never implements world features, full-Noodle error messages, or tooling.
- **The bootstrap's language** is an owner decision (§74). Propose one that is small, boring, widely available, and easy to audit, with reasons. It matters little, because it will be retired.
- The bootstrap MAY include one minimal test mode (run a program, compare its output with an expected file). The real test runner is written in Noodle.

```text
Stage −1 (optional, purist): auditable hex seed ──┐
Stage 0: tiny bootstrap interpreter (Noodle Seed) ┘
        │ runs
        ▼
Noodle compiler + World VM, written in Noodle Seed
        │ compiles itself
        ▼
compiler₁ ──► compiler₂ ──► compiler₃      compiler₂ ≡ compiler₃, bit for bit
        │
        ▼
SELF HOSTING ACHIEVED ──► bootstrap frozen and retired
        │
        ▼
Every release N is built by release N−1
```

**The N−1 rule.** After self-hosting, the compiler's source may only use features the previous release supports. The build chain never breaks.

**Build from nothing (optional, late).** Study the bootstrappable-builds work (stage0-style hex seeds, full-source bootstrap). Aim for a documented chain in which a few hundred bytes of hand-auditable machine code build, stage by stage, a Noodle that builds today's Noodle — with no other programming language anywhere in the chain. Document it in `BUILD_FROM_NOTHING.md`. This is the ultimate form of "purely Noodle."

**Trusting trust.** Read Ken Thompson's "Reflections on Trusting Trust" and David A. Wheeler's work on Diverse Double-Compiling. Both shape the proof in §49.

### §6 — Honesty and evidence

- If a feature isn't implemented, say so. If it's experimental, label it experimental in the CLI, the docs, and its errors.
- Never fake output, benchmarks, test results, or demos. Never write placeholder code that makes tests pass.
- Never write tests that assert nothing, skip silently, or copy the implementation's bugs. Never weaken or delete a test without a decision record explaining why.
- Every claim in docs, commits, or reports points to evidence: a test name, a command and its output, or a benchmark file.
- The words "done," "works," "complete," "fast," "secure," and "self-hosted" never appear in a report without evidence attached.
- Never claim "Noodle is the first language to do X" unless it has been rigorously established (§68).

### §7 — The North Star demo

Every decision should move Noodle toward this scene being real:

1. A beginner writes `bank.noodle` in plain English.
2. They run it. On line 12, the balance goes to −20.
3. Noodle explains exactly how it happened: which lines, which values, which inputs.
4. They rewind to a checkpoint, add `protect balance must never be below 0`, and replay the same recorded inputs through the fixed code.
5. Noodle shows the old run and the fixed run side by side.
6. They ask `what if interest is 5` and see only what would change.
7. They save the world, close the laptop, and resume it tomorrow.
8. Every tool they used — compiler, runtime, world engine, REPL — was written in Noodle and built by Noodle.

As soon as its pieces exist, this scene becomes an end-to-end test in `conformance/north_star/`.

---

## PART II — THE LANGUAGE

### §8 — Read the specification first

Before changing anything, **read the entire Noodle specification** if one is supplied (as a file or inside an existing repository). Treat it as the source of truth for existing syntax and semantics. Do not casually redesign it. Do not silently remove syntax. Do not replace its philosophy with conventional language design.

**If no separate specification is supplied,** derive a draft (`spec/SPEC.md`) from this document, mark every syntax decision as PROPOSED, and list open questions for the owner. Never invent silently.

The specification MUST eventually contain:

- the grammar in a formal notation of your choice, with a precedence table;
- the precise meaning of every construct — what it does, what it records in history, which errors it raises;
- the value model, scoping rules, and evaluation order;
- world-model semantics: state, events, history, provenance, checkpoints, branches, experiments, rewind, replay, effects;
- the Seed subset (`spec/SEED.md`);
- the error catalogue;
- the keyword budget (§17);
- a version number and a changelog.

### §9 — Surface principles (preserved)

The original Noodle principles are non-negotiable:

- English-like programming
- no semicolons
- no curly braces
- no parenthesized commands
- no explicit type declarations
- no imports
- no classes, no objects, no `self`, no pointers
- indentation is not required for program meaning
- one instruction per line
- case-insensitive keywords
- friendly errors, no scary stack traces
- simple, beginner-friendly syntax

A beginner must be able to understand this without knowing any other language:

```noodle
say hello

make money is 100

if money is bigger than 50
    say "You have enough"
otherwise
    say "You need more"
end
```

The language must never become a wall of punctuation. Prefer English-like constructs. Avoid syntax that exists merely because another language has it. Every keyword must justify its existence.

### §10 — The Ambiguity Doctrine

English is ambiguous. Noodle must not be.

- Every line has **exactly one** meaning. If two readings exist, it is an error that shows both readings and suggests a rewrite. Noodle never silently picks one.
- Define and document precedence for `and`, `or`, `not`, comparisons, and arithmetic. Where mixing `and` with `or` could confuse, require a clarifying form (design one — candidates: `both … and …`, `either … or …`) or splitting the line.
- `is` appears in binding (`make money is 100`) and in comparison (`if money is 100`). The grammar MUST separate these by context, and errors must teach the difference.
- Decide whether names may contain spaces (`make high score is 10`). If yes, define exactly how names are told apart from keywords. If no, the error must suggest the alternative.
- Decide case rules for names (keywords are case-insensitive — are names?). Normalize Unicode names.
- Decide how long lines continue, and what bare words mean (`say hello` versus `say "hello"`) — per the specification.
- The parser reports several independent errors per run where helpful, but never cascades nonsense.

### §11 — Existing core features (preserve and implement)

Comments · `say` · `ask` · `make` · numbers · words (text) · yes/no · lists · arithmetic · comparisons · `and` · `or` · `not` · `if` · `otherwise` · `end` · `repeat` · `for each` · `add` · `teach` · `do` · `give back`

The exact semantics follow the specification. For each feature, the spec answers: every accepted form, how it evaluates, which errors it raises, and which event it records in the world's history. Existing examples must keep working forever — they become conformance tests on day one.

### §12 — Values

Where the specification already defines these, it wins. Otherwise, propose these defaults through NEPs:

- **Numbers are exact.** `0.1 plus 0.2` is `0.3`. Whole numbers never overflow. Decide how non-terminating results such as `1 divided by 3` are represented (exact fractions, or decimals with a documented precision and rounding rule). Dividing by zero is a friendly error.
- **Approximate numbers** (later, optional): if fast floating point is ever needed, it is opt-in and visible (candidate: `about 3.14`), and Noodle's own math routines keep results identical on every platform.
- **Text is Unicode** (source files are UTF-8). `length of` counts what a human sees — characters as displayed, not bytes. The Unicode tables are generated from official Unicode data by a Noodle tool.
- **yes and no** are the only truth values. No "truthy" surprises: `if 0` is an error with a suggestion, not a silent `no`.
- **Nothing:** decide whether an empty "nothing" value exists. If it does, using it by mistake produces a friendly error that points — through provenance — to where it came from.
- **Values never change; variables do.** "Changing" a list creates a new version of it that shares structure with the old one. This single rule makes checkpoints cheap, branches safe by construction, history honest, and memory management simpler (§37).

### §13 — New language features

Everything below is **candidate syntax**. For each feature — and for the original advanced capabilities (remember, history, why, checkpoint, branch, experiment, rewind, replay, compare, watch, protect, show state) — first design precise semantics, then choose the best beginner-friendly syntax. Do not blindly adopt the keywords shown here.

A feature lands only with: a NEP (§17), a passed Beginner Test (§16), spec text, tests, docs, an example, and friendly errors. Records and kinds are **data only** — no methods, no `self`, no inheritance — so "no classes, no objects" stands. No feature may introduce braces, semicolons, or type declarations. Build the tiers in order.

#### Tier 1 — Expressiveness
*Needed for real programs, and for writing the compiler in Noodle.*

**1. Records** — named bundles of values.
```noodle
make player with
    name is "Mo"
    score is 0
end
say player's name
make player's score is player's score plus 10
```

**2. Tables** — look values up by a key (final name via NEP: table, dictionary, or lookup).
```noodle
make prices is a new table
put 30 in prices under "apple"
say prices under "apple"
```

**3. Kinds** — a value that is one of several shapes. Essential for writing a compiler cleanly (tokens, syntax trees).
```noodle
a shape is one of
    a circle with radius
    a square with side
end

make ball is a circle with radius 5
```

**4. choose / when** — branch on values and kinds.
```noodle
choose ball
    when it is a circle
        say "round, radius [its radius]"
    when it is a square
        say "boxy"
    otherwise
        say "something else"
end
```

**5. Richer loops** — `repeat 5 times`, `repeat while …`, `repeat until …`, `count from 1 to 10`, with `stop` (leave the loop) and `skip` (go to the next round).

**6. Text** — `length of`, `uppercase of`, `lowercase of`, `… contains …`, `… starts with …`, `split … by …`, `join … with …`, and text templates:
```noodle
say "Hi [name], you have [money] coins"
```

**7. Lists** — `first item of`, `last item of`, `item 3 of`, `remove … from …`, `sorted`, `reversed`, `count of`, `sum of`, `average of`, `largest of`, `smallest of`, plus sentences that filter and transform:
```noodle
make passing is every score in scores where score is at least 50
make doubled is every number in numbers turned into number times 2
```

**8. Failure handling a beginner can read:**
```noodle
try
    make age is the number in answer
if that fails
    say "That wasn't a number: [the problem]"
end
```

**9. Recursion** inside `teach`, with a depth limit and a friendly "this went too deep" error that shows the repeating calls.

**10. Validated asking** — asking for a number re-asks politely until the answer is valid. Every answer is recorded for replay.

**11. Randomness and time as recorded effects** — `pick a random number from 1 to 6`, `wait 2 seconds`, `the time now`. Seeded, journaled, replayable (§22). During replay, waits take no real time.

**12. Bytes and bits** — byte lists and bit operations with English names (candidate: `shift … left by …`), needed by the compiler backend and kept out of beginner levels.

#### Tier 2 — World and time (Noodle's signature)

**13. remember** — values that survive between runs, stored in the project's own memory file together with their history.
```noodle
remember best_score
```

**14. why** — explain a value from the real causal graph.
```noodle
why is profit 500
why did money change
```
The answer lists the lines, inputs, and earlier values that actually produced it — generated from recorded provenance, never invented.

**15. what if** — a counterfactual that recomputes only what depends on the change, reports the differences, and leaves the real world untouched.
```noodle
what if price is 20
    show profit
end
```

**16. Questions about time** — history is data the program can ask about.
```noodle
say money as it was at checkpoint before_sale
say money 3 changes ago
say how many times money changed
say when money first became bigger than 100
```

**17. whenever** — reactive rules that fire on real state changes (event-driven, never polling). Ordering, re-entry, and loop protection are defined precisely.
```noodle
whenever balance is below 0
    say "Careful, you're overdrawn"
end
```

**18. compare and merge** — worlds behave like versions.
```noodle
compare branch cheaper_prices with main
merge branch cheaper_prices into main
```
When the same value changed differently on both sides, Noodle stops, explains the conflict in plain words, and offers choices. It never resolves a conflict silently (§26).

**19. undo and time-travel stepping** in the REPL and debugger: `undo`, `step back`, `step forward`, `go to step 42`, `go to when money changed`.

**20. Replay with fixed code** — record a run, edit the program, replay the same recorded inputs through the new code, and compare the two worlds. Noodle shows exactly where the runs diverge. Every bug report becomes a reproducible experiment.
```text
noodle replay bank.run --with-current-code --compare
```

**21. Save and resume a running world** — pause a program, save its complete world (state, history, execution position), and resume it later, even on another machine.
```noodle
save the world to "bank.world"
```
```text
noodle resume bank.world
```

#### Tier 3 — Correctness built into the language

**22. Tests in plain English,** run by `noodle test`:
```noodle
test "adding works"
    check 2 plus 2 is 4
end
```

**23. Examples that are also tests,** written inside teachings:
```noodle
teach double number
    for example double 4 gives back 8
    give back number times 2
end
```

**24. Promises on teachings** — `needs amount is bigger than 0`, `promises balance is at least 0`. A broken promise produces a friendly error backed by provenance.

**25. Properties** — `for any whole number n, check n plus 0 is n`. Noodle generates many inputs (recorded and replayable) and shrinks any failure to the smallest example.

**26. Stronger protections** — `protect age must always be a number`, `protect score must never go down`, checked on every transition that touches the protected value.

#### Tier 4 — Later
*Only after SELF HOSTING ACHIEVED (§49), each with a NEP, evidence of real need, and owner approval.*

**27. Worlds as the concurrency model** — isolated worlds that share nothing and communicate only by messages delivered in a deterministic, recorded order. No threads, no locks, no data races — and still replayable.

**28. explore** — systematically try every answer (within limits) to a program's `ask`s and random choices, and report any path that breaks a protection. Bounded model checking, powered by native branching.

**29. Levels** — progressive disclosure for learners (`noodle --level 2`). Errors say "that's a level 4 feature" and explain it. With owner approval, this may come earlier if beginner testing shows the need.

**30. Language packs** — keywords in other human languages (for example Hindi or Spanish). One language-neutral syntax tree, many views; `noodle translate` shows any program in any installed pack. Packs are Noodle files; English remains the reference.

**31. Units of measure** — `make distance is 5 km`, `make time is 2 hours`; `say distance divided by time` shows `2.5 km per hour`; adding km to seconds is a friendly error.

**32. Hot reload** — edit a running program; the world keeps its state; history records the code change as an event.

### §14 — Projects without imports

"No imports" stands. Multi-file programs — the Noodle compiler above all — still need a precise model:

- A **project** is a folder. Every `.noodle` file inside it shares one world of names. No import statement exists anywhere.
- The project file `project.noodle` is written in Noodle, not JSON, TOML, or YAML. It holds the name, version, entry file, edition, permissions, and resource budgets. Candidate:
```noodle
this project is called "bank"
start with "main.noodle"
this program may read files in "data"
```
- A name defined in two places is an error that lists every definition and suggests a fix. Nothing shadows silently.
- Decide by NEP whether folders form neighborhoods of names (for example, shared within a folder and visible elsewhere only when marked `share`). The compiler's own size will test this decision.
- Shared libraries ("recipes") are copied into the project's `recipes/` folder by the CLI and pinned by content hash in `recipes.lock.noodle`. Recipes receive zero capabilities unless the project explicitly grants them.

### §15 — The error system

Errors are part of the language design. Never show `Traceback`, `SyntaxError`, `NullPointerException`, `Segmentation fault`, or internal compiler names. Prefer:

```text
Line 8:
I expected a number here.

You wrote:
money is blue

Try using a number, such as:
money is 100
```

Every error SHOULD include: the line number, the source line with a marker under the problem, a plain explanation, a suggested fix (ideally a corrected line), and a short code such as `N0042` that `noodle explain N0042` expands into a mini-lesson.

**Upgrades:**

- **Did you mean?** — suggestions for misspelled names and keywords.
- **Errors that tell the story** — runtime errors use provenance to show *how* the bad value came to be:
```text
Line 12:
balance would have become -20.

That breaks your rule on line 3:
balance must never be below 0.

How it happened:
line 30 did withdraw 120
while balance was 100 (set on line 8)

Try checking the amount before line 12.
```
- **Internal errors** (bugs in Noodle itself) are friendly too: apologize, save a crash report with full technical detail for developers, and explain how to report it. Stack traces go into that file, never onto the screen.
- **One catalogue.** Error text lives in Noodle data files: testable with exact-text golden tests, translatable, and identical across the CLI, the REPL, the editor, and every execution engine.

### §16 — The Beginner Test and the Advanced Test

**The Beginner Test.** Give Noodle to a beginner. Can they understand `say hello` without knowing another language? Write `make score is 10` without understanding types? Read an `if … end` block without learning braces? Fix a broken program using only the error message? If not, redesign the surface.

- Make it evidence-based: before finalizing any keyword, check it with real beginners (five or more when possible; documented heuristics when not). Learn from evidence-based language design research (the Quorum project) and gradual teaching languages (Hedy).
- Keep `BEGINNER_LOG.md`: every confusion observed, and what changed because of it.

**The Advanced Test.** An experienced programmer must be able to build serious things — utilities, games, simulations, data processing, automation, developer tools, educational programs, stateful applications, world simulations — without artificial walls and without another language for the core logic. The ultimate advanced test is Noodle itself.

### §17 — Language evolution: NEPs and the anti-bloat rule

Every new feature needs a **Noodle Enhancement Proposal** (`neps/NEP-0001-title.md`) answering:

1. What problem does it solve?
2. Why does it belong in Noodle?
3. Can a beginner understand it? (evidence)
4. Does it need new syntax, or can existing syntax do it?
5. What does it cost the runtime, the world model, and determinism?
6. What does it record in history and provenance?
7. Which errors can it produce, and what exactly do they say?
8. What is the compatibility impact?
9. Can the same problem be solved more simply?
10. Does it fit Noodle's philosophy?
11. Which tests and examples prove it?

Reject unnecessary complexity. Do not copy every feature from every language. Noodle's strength is:

```text
SIMPLE LANGUAGE + POWERFUL EXECUTION MODEL + SELF-HOSTING + HUMAN-FRIENDLY EXPERIENCE
```

The specification keeps a **keyword budget**: every keyword, listed with the NEP that introduced it and the beginner evidence behind it.

---

## PART III — THE WORLD MODEL

### §18 — The central principle

Noodle's power is not its syntax. It is what happens when a program runs:

```text
Code → Run → Remember → Understand → Experiment → Branch → Rewind → Continue
```

Every program runs inside a **world**. The original advanced capabilities — `remember`, `show history`, `why`, `checkpoint`, `branch`, `experiment`, `rewind`, `replay`, `compare`, `watch`, `protect`, `show state` — and the new world features of §13 are all views of that one model. They MUST be real runtime capabilities built on recorded data. Never simulate them, never reconstruct them for display, never approximate them with guesses.

**If the world is fake, Noodle is fake.**

Everything in this Part is written in Noodle (the World VM and the world engine) and runs on the bootstrap during early development (§5).

**Vocabulary.** The specification defines each term precisely.

| Term | Meaning |
|---|---|
| World | Everything about one running program: code, state, position, history, and more (§19) |
| State | Every name and its current value (§21) |
| Step | One numbered unit of execution (§20) |
| Transition | One change to the state, made by one step (§21) |
| Effect | Anything that crosses the world's edge, in or out (§22) |
| Journal | The ordered record of every effect (§22) |
| History | The ordered record of every transition and effect (§23) |
| Provenance | The recorded causes of every value (§24) |
| Checkpoint | A named snapshot of the whole world (§25) |
| Branch | An alternative timeline (§26) |
| Experiment | A temporary branch, discarded unless kept (§27) |
| Rewind | Moving the live world back to an earlier point (§28) |
| Replay | Re-running from recorded inputs to reproduce a world exactly (§29) |
| Observer | A `watch` or `whenever` rule (§30) |
| Protection | A rule the state must always satisfy (§31) |

### §19 — The world

```text
WORLD
├── code          the exact program version running (content hash)
├── state         every name and its current value
├── position      file, line, step, teachings in progress, loop rounds
├── history       every transition and effect, in order
├── provenance    the causes of every value
├── journal       every effect that crossed the world's edge
├── checkpoints   named, content-addressed snapshots
├── branches      alternative timelines, including abandoned ones
├── observers     watch and whenever rules
├── protections   rules the state must always satisfy
└── grants        the capabilities this program was given (§41)
```

**A world is version control for a running program.** Design it that way:

| Version control | Noodle world |
|---|---|
| a version of the files | a state |
| a commit | a checkpoint |
| a branch | a branch |
| the log | history |
| blame | `why` |
| diff | `compare` |
| merge | `merge` |

Study Git's content-addressed object model — without depending on Git. A world is also data: it can be saved (§13 #21), resumed, compared, and inspected by tools (§32), using the formats of §39.

### §20 — Reified execution

Rewind, save-and-resume, and replay are honest only if *where the program is* is data, not a hidden detail of the machine.

- The execution position — file and line, the teachings in progress and their local names, loop counters, pending `whenever` rules — lives in the world as an ordinary Noodle data structure. Rewinding or saving never has to capture a host call stack, because there isn't one to capture.
- Study explicit-state abstract machines (CEK-style machines, defunctionalized interpreters) before designing the World VM.
- The specification defines exactly what one **step** is. Steps are numbered from 1 in every run. Step numbers appear in history, provenance, journals, errors, and tools, and they MUST be identical on every engine (Law 9). Native code (§36) keeps a step counter even when it records little else, so step-based limits (§42) and step numbers always agree.

### §21 — The state model

- **State** maps names to immutable values (§12), including remembered values and the local names of teachings in progress.
- Every change is a **transition**, recorded with: step, file and line, name, old version, new version, causes (§24), the effect it came from (if any), and the branch.
- Because values never change, "changing" a list creates a new version that shares structure with the old one. Use persistent data structures — hash array mapped tries for tables, RRB-trees for lists — so keeping old versions is cheap.
- Every value version has a stable identity that history, provenance, and checkpoints can point to.
- `show state` lists names and values in a deterministic, friendly order. Large values are summarized, with a way to see more.

### §22 — The effect boundary

Everything that crosses the world's edge is an **effect**, and every effect passes through **one effect gateway** in the runtime. That single door is where capabilities are checked (§41) and the journal is written. Nothing reaches the operating system any other way.

| Kind | Examples | Normal run | Experiments, `what if`, other branches | Replay |
|---|---|---|---|---|
| Pure | arithmetic, text, lists, tables, teachings without effects | Computed | Computed in isolation | Recomputed |
| Recorded input | `ask`, reading files, randomness, the time now, command-line arguments, reading remembered values, network responses | Performed, then journaled | Performed live and labeled with the experiment's name, or served from an answer supplied in advance | Served from the journal — never performed again |
| Screen output | `say`, `show` | Shown and journaled | Shown, labeled with the experiment's name | Re-derived and compared with the journal; shown only in replay views |
| Reversible output | writing files inside granted folders, changing remembered values | Performed with enough saved to undo it (such as the previous contents), committed atomically (§40), journaled | Held in the experiment's copy-on-write overlay; applied only on keep or merge | Re-derived and compared; never written again |
| Irreversible output | sending network data, writing or deleting outside the sandbox, running other programs | Performed only with a grant; journaled and marked irreversible | Never performed. Captured and reported: *"this would have sent …"* | Never performed again |

Rules:

- Anything Noodle can't undo is irreversible by definition. Be honest about it: after a rewind past an irreversible effect, say so plainly — *"The message sent at step 57 can't be unsent."*
- Irreversible effects happen only on the live timeline, never inside experiments, `what if`, replay, or branches that aren't live.
- Randomness comes from a pseudo-random algorithm written in Noodle and specified exactly; its seed is journaled. Time comes from a clock the world controls: during replay it is virtual, and `wait` takes no real time.
- **Every run produces a journal** from gate G3 onward. `noodle run` keeps recent run records — code hash, Noodle version, seeds, and journal — in the project's `.noodle/runs/` folder, within the budgets of §42. `noodle run bank.noodle --record bank.run` keeps one under a chosen name.
- Document the effect model in `docs/EFFECTS.md`.

### §23 — History

- History records every transition and every effect, in order, each with its step, file, line, and branch. It is a directed acyclic graph, not a line: a merge has two parents (§26).
- Programs can ask about history (§13 #16) and show it: `show history`, `show history of money`.
- **Retention.** History is never silently lost — it is compacted. Anything removed from memory MUST remain reconstructible by replaying the journal from a checkpoint. Compaction is reported, never hidden.
- **Recording levels:**

| Level | Keeps | Used for |
|---|---|---|
| `full` | every transition, value version, and provenance edge | learning, the REPL, the debugger, `why` |
| `history` | every transition, without fine-grained provenance | long-running programs that still ask about history |
| `light` | the journal and periodic checkpoints; everything else is rebuilt by replay when asked | production-style runs |

- The compiler raises the level automatically when a program asks questions that need more (`why`, `how many times … changed`, `as it was at …`). Tools such as the debugger run at `full`.
- The level changes what is kept, never what happens (Law 7).
- History budgets for memory and disk are set in plain English in `project.noodle`. Reaching a budget triggers compaction with a friendly notice — never a crash, and never the loss of anything the program asked about.

### §24 — Provenance

Provenance is a **causal graph**. Each value version points to the step that produced it, the operation, the line, the value versions it was computed from, and any effect it came from (for example, the `ask` that supplied it).

- **Sound:** every edge is a dependency that actually happened. Nothing is inferred, guessed, or invented.
- **Complete, including control flow:** a value set inside `if money is bigger than 50` depends on the `money` that chose that path. The specification defines exactly which control dependencies count.
- `why` answers (§13 #14) are produced from this graph by deterministic templates: the shortest faithful explanation first, with a way to see the full chain. Never by an AI model (§3).
- The graph answers in both directions: backward (*why is profit 500?*) and forward (*what depends on price?* — which `what if` uses to recompute only what depends on a change).
- Internally, answer provenance questions with dynamic slicing and simple Datalog-style queries over the graph. Keep the representation compact: share structure, and at lower recording levels rebuild details on demand from the journal.
- Verify provenance with differential tests against an independent, deliberately simple reference implementation of dynamic slicing (§52).

### §25 — Checkpoints

A checkpoint is a named snapshot of the entire world: state, position, journal position, overlay contents, observers, and protections.

```noodle
checkpoint before_sale
rewind to before_sale
show checkpoints
```

- **Cheap:** because values are immutable, a checkpoint shares almost everything with the live state.
- **Content-addressed:** each checkpoint is identified by a hash of its contents (SHA-256, implemented in Noodle and verified against official test vectors). Identical worlds produce identical hashes.
- **Automatic checkpoints** are taken at the start of a run, before every `ask`, before every irreversible effect, and periodically in long runs, so rewind and replay stay fast.
- Checkpoints survive saving the world and move between machines (§39).

### §26 — Branches and merge

A branch is an alternative timeline that starts at a checkpoint or at the current step.

```noodle
checkpoint before_sale

branch cheaper_prices
    make price is 20
    do sale
end

compare branch cheaper_prices with main
merge branch cheaper_prices into main
```

- **Isolation** (Law 4): nothing done on one branch is visible on another except through an explicit merge.
- **Compare** lists every name whose value differs, with each side's provenance, so the difference is explained, not just listed.
- **Merge** is three-way, using the common ancestor:
  - changed on one side only → take that change;
  - changed the same way on both sides → take it;
  - changed differently on both sides → **conflict.** Noodle stops, explains the conflict in plain words, and offers choices: keep this side, keep that side, or write the value yourself. It never resolves a conflict silently.
  - Records merge field by field, and tables key by key. Lists merge only when at most one side changed them, unless a NEP defines something safer. Everything else is a conflict.
- Files in a branch's copy-on-write overlay merge by the same rules, file by file.
- Protections (§31) are re-checked on the merged state. A merge that would break a protection is refused, with the story of why.
- A merge is recorded in history as one event with both parents.

### §27 — Experiments

An experiment is a temporary branch: run something in isolation, see what it changes, then keep it or throw it away.

```noodle
experiment double_prices
    make price is price times 2
    show profit
end

keep experiment double_prices
```

```text
main ──●───────────────────────────●──► continues untouched
        \                          ↑
         └── experiment ──●──●──●──┘ keep (merge, §26) — or discard, the default
```

- `keep experiment …` merges the experiment's changes into the main world (§26). An experiment that isn't kept is discarded, and the main world is exactly as it was (Law 5). History MAY note that it ran.
- `what if` (§13 #15) is a lightweight experiment that recomputes only what depends on the change, using provenance and incremental computation (study Adapton and Salsa).
- Irreversible effects are never performed inside an experiment. The report lists them as *"would have …"*.
- Inputs inside an experiment are recorded as the experiment's own inputs and labeled as such.

### §28 — Rewind

Rewind moves the live world back to a checkpoint or step: `rewind to before_sale`, `rewind 3 steps`, and — in the REPL and debugger — `undo`, `step back`, and `go to step 42` (§13 #19).

**Rewind never destroys anything.** The timeline you rewound away from is kept as an abandoned branch, so a rewind can itself be undone:

```text
A ── B ── C ── D        rewind to B
A ── B                  the live world continues from B
      └── C ── D        kept as branch "abandoned-1"
```

- Rewind restores the state, the position, the observers, and every reversible output (§22).
- Rewind never pretends to undo an irreversible effect. It names each one it can't undo.
- Rewinding to a checkpoint and replaying the same journal reproduces the same world (Law 2).

### §29 — Replay

Replay re-executes a program from the start or from a checkpoint, feeding it recorded inputs from the journal, and reproduces the identical world: the same state, output, history, and hashes (Laws 2 and 6).

- Nothing outside the world may influence a run: the clock is virtual, randomness comes from the journaled seed, tables keep insertion order (never hash order), and nothing depends on memory addresses, timing, locale, or the host platform.
- **Divergence is reported, never hidden.** If a replay stops matching its record — because the code changed, or because Noodle has a bug — Noodle shows the first step where the runs split, both values, and both chains of causes:

```text
The runs split at step 214, on line 12 (called from line 30).
Old run: balance became -20.
New run: that change was refused by your rule on line 3:
balance must never be below 0.
```

- **Replay with fixed code** (§13 #20) matches recorded inputs to the new program by order and by the identity of each `ask` (its question and its line). If the new code asks for something the journal doesn't have, Noodle stops and asks the user live, clearly labeled. It never invents an answer.
- Run records (`.run`) are portable across machines, and across Noodle versions within an edition (§39, §63). Document replay in `docs/REPLAY.md`.

### §30 — Observers

- `watch money` reports every change to `money`: old value, new value, line, and cause.
- `whenever` rules (§13 #17) react to real state changes. They are event-driven hooks inside the World VM, never polling.
- The specification defines precisely: when observers run (after the transition that triggers them), in which order (declaration order), how their own changes are recorded (as transitions whose cause is the rule that fired), and how re-entry and endless chains are stopped (with a friendly error that shows the loop).
- Observers are part of the program, so replay fires them exactly as the original run did. Inside an experiment, observers fire within the experiment.

### §31 — Protections

- A protection is a rule the state must always satisfy: `protect balance must never be below 0`, `protect age must always be a number`, `protect score must never go down` (§13 #26). Rules such as "never go down" compare the old and new versions of a value.
- The compiler works out which transitions could affect each protection; the runtime checks every one of them, on every engine, at every recording level. Protections are never skipped for speed.
- **A violating transition is refused.** The world keeps its last valid state, an automatic checkpoint is taken, and the program stops with a story error (§15) that shows the value it would have become and how. Inside `try`, the refusal is the problem that `if that fails` handles (§13 #8).
- Protections apply everywhere: the live run, branches, experiments, `what if`, merges (§26), and replay with fixed code — which shows exactly where a new rule would have fired in an old run (the North Star, §7).

### §32 — Self-inspection

A world can be inspected by the program itself, by the REPL, and by tools:

- `show state`, `show history`, `show checkpoints`, `show branches`, `show protections`, `show watchers`, `why …`, `how many times … changed`.
- `noodle timeline bank.run` — a text timeline of steps, changes, checkpoints, branches, and effects. A visual version comes later, in the playground and the editor.
- `noodle story bank.run` — a plain-English narration of a run, generated from history by deterministic templates: *"You started with money at 100 (line 3). You asked for a price and got 30 (line 7). …"*
- Inspection is read-only and deterministic: inspecting a world never changes it, and inspecting the same world twice gives the same answer.
- Noodle's own compiler runs in a world too. Use it: replay compiler bugs, and ask `why` about the compiler's own values. Noodle debugs Noodle.

### §33 — The World Laws

These ten laws are the contract of the world model. Laws 1–9 are enforced by property tests (§52) that generate random programs, inputs, checkpoints, and rewind points, on every engine. Law 10 is enforced by `noodle selfhost verify` (§49).

1. **Event sourcing.** The current state is exactly what you get by applying the recorded transitions, in order, from the start or from any checkpoint.
2. **Rewind–replay.** Rewinding to any checkpoint and replaying the same journal reproduces the same world: state, output, history, and hashes.
3. **Checkpoint.** Restoring a checkpoint yields exactly the world that was saved, and a checkpoint's hash identifies its contents.
4. **Isolation.** Nothing done in a branch, experiment, or `what if` is visible in any other timeline without an explicit merge or keep.
5. **Discard.** Discarding an experiment leaves the main world's state exactly as if it never ran.
6. **Determinism.** The same program with the same journal produces the same world on every supported machine.
7. **Neutrality.** Recording levels, observation by tools, and inspection never change what a program does or produces.
8. **Provenance.** Every value version has a chain of causes that ends in literals, recorded inputs, or remembered values — and every `why` answer comes only from that recorded chain.
9. **Engine.** The World VM and native code produce identical observable behavior — output, errors, step numbers, history, journals, and `why` answers — for every conformance program (§38).
10. **Fixpoint.** The Noodle compiler, compiling itself, reaches a bit-identical fixpoint (§49).

A change that breaks a law is a bug, however convenient it would be.

---

## PART IV — THE IMPLEMENTATION

### §34 — Architecture

```text
source files (.noodle, UTF-8)
   │
   ▼
lexer → parser → lossless syntax tree (keeps comments, spacing, and positions)
   │
   ▼
name resolution — projects without imports (§14)
   │
   ▼
meaning checks and inference — no type declarations
   │
   ▼
Noodle IR — the one contract between the front end and every engine
   │
   ├──► World VM — the reference engine, with every world feature
   │
   └──► native backend → Noodle assembler → Noodle linker → executable
                                  (runtime and world engine linked in)
```

Everything in this diagram is Noodle source.

- **One front end for every tool.** The compiler, formatter, linter, language server, and debugger share one lexer, one parser, and one error catalogue.
- **Inference without declarations.** The compiler works out what kind of value each name holds wherever it can, to catch mistakes before running and to generate good code. What can't be known until run time is checked at run time. Either way, the error reads the same.
- **The IR** has a documented textual form (`docs/IR.md`), so it can be inspected and golden-tested.
- **Incremental compilation** comes later, for the editor: query-based recomputation in the style of Salsa.
- The runtime — values, memory, the effect gateway, the world engine, and the standard library — is Noodle, linked into every native executable.

### §35 — The Machine Layer

At the very bottom, a runtime must touch raw memory, machine words, and system calls. Other languages drop into C or assembly there. Noodle doesn't.

The **Machine Layer** is a privileged dialect of Noodle: the same plain-English syntax, plus a few extra powers — raw memory, fixed-size machine numbers, addresses, system calls, and emitting specific instructions. It is allowed **only** in files inside `noodle/machine/` whose first line is `this file works with the machine`.

*Sketch:*

```noodle
this file works with the machine

teach send bytes to the screen
    needs bytes
    make start is the address of bytes
    make size is the length of bytes
    give back system call "write" with 1, start, size
end
```

- The compiler refuses Machine Layer powers anywhere else. User programs can never use them, so "no pointers" stays true for everyone writing Noodle outside Noodle's own core.
- Keep it tiny. Whatever can be ordinary Noodle is ordinary Noodle. The rest of the runtime reaches the machine only through a small, documented set of Machine Layer teachings, each with a written safety contract and its own tests.
- `noodle purity` reports every Machine Layer file and its size (§4). Growth needs a decision record.
- Precedents: Go's runtime, written mostly in Go; Oberon's operating system, written in Oberon with a `SYSTEM` module for low-level access; Jikes RVM and Maxine, Java virtual machines written in Java; RPython, the restricted Python in which PyPy is written. Noodle goes one step further: even the final instructions are emitted by Noodle's own backend, never by an external assembler.

### §36 — The native backend

Noodle compiles to machine code with its own code generator, assembler, and linker — all written in Noodle, all producing deterministic bytes.

Targets, in order:

1. **Linux x86-64** — static ELF executables, no libc, direct system calls. The first target: the simplest honest OS door.
2. **Linux AArch64.**
3. **macOS AArch64** — Mach-O executables that reach the operating system through libSystem, the only supported door on macOS (raw system calls are not a stable interface there). Apple Silicon runs only signed code, so Noodle's linker MUST write an ad-hoc code signature itself — no `codesign` tool.
4. **Windows x86-64** — PE executables that import only from the system DLLs.
5. **WebAssembly** — modules emitted directly, for the playground (§58) and other sandboxed hosts.

Rules:

- **Deterministic output:** the same input produces the same bytes. No timestamps, random identifiers, or host-dependent ordering.
- **Cross-compilation from the start:** every supported host builds for every target, producing the same bytes (§51).
- **Correct first, fast later:** begin with simple, obviously correct code generation. Optimize only behind benchmarks (§45).
- **Inspectable:** `noodle disassemble` shows generated code next to the Noodle lines it came from. Test the assembler with round trips (assemble, disassemble, reassemble) and with golden instruction encodings taken from the official architecture manuals.
- Debug information for native debuggers (DWARF, PDB) is optional and later. Noodle's own time-travel debugger (§57) comes first.

### §37 — Memory management

Noodle has no pointers for users, and values never change, so memory management is fully automatic.

- **Candidate:** reference counting with reuse — when a value has exactly one owner, an "update" reuses its memory in place. Study Lean 4 ("Counting Immutable Beans") and Koka (Perceus). The alternative is a tracing collector. Decide with a decision record backed by benchmarks.
- **The insight to prove:** if every value is immutable and can only refer to values that already existed when it was created, reference cycles cannot form, and reference counting alone reclaims everything. Prove this against the final semantics. If any feature could ever create a cycle, find it and handle it before relying on the insight.
- History keeps old versions alive on purpose. Memory is reclaimed through retention and compaction (§23), never by quietly dropping history a program asked for.
- The allocator lives in the Machine Layer (§35) and is written in Noodle. Running out of memory produces a friendly error (§42), never a crash.

### §38 — Two engines, one meaning

The World VM and native code are two engines for one language.

- Every conformance program runs on both, and their observable behavior MUST be identical: output, exact error text, step numbers, history, journals, and `why` answers (Law 9). Any difference is a bug — never "a platform difference."
- The World VM is the executable reference. Where the VM and the specification disagree, the specification wins and the VM is fixed.
- Native code may optimize only where Law 7 guarantees nobody can tell — for example, by recording less at the `light` level.
- Exact numbers (§12) and Noodle's own math routines make identical results possible on every platform.
- Native executables include the world engine and write the same journal format as the VM.

### §39 — Storage

Noodle stores remembered values, run records (`.run`), saved worlds (`.world`), checkpoints, and caches.

- Every format is designed by Noodle, documented in `docs/FORMATS.md`, and versioned in its header. A newer Noodle reads older files within an edition and upgrades them explicitly. An older Noodle refuses newer files with a friendly message.
- Formats are byte-for-byte identical on every platform: fixed byte order, no platform-sized numbers.
- Every record carries a checksum. Content addresses use SHA-256, implemented in Noodle and verified against the official test vectors.
- No database engine. Noodle's own append-only logs and indexes are enough.
- Remembered values live with their project (candidate: `.noodle/memory`), never in a hidden global place the user wasn't told about.
- `noodle inspect bank.world` prints any stored file in readable form.
- **Privacy:** run records and saved worlds can contain everything a user typed. Say so in the docs, and provide a way to redact recorded inputs before a run is shared as a bug report.

### §40 — Crash recovery

- Every save is atomic: write to a temporary file, flush it to disk, rename it into place, and flush the folder where the platform requires it.
- Journals are append-only, with a checksum on every entry. After a crash, Noodle finds the last valid entry, drops the torn tail, and says exactly what it recovered: *"I recovered your world up to step 1,204. The last half-written change was dropped."*
- Remembered values are never half-updated: after any crash, each one holds either its old version or its new one.
- Irreversible effects are journaled as an intention before they happen and confirmed afterward. If a crash falls between the two, Noodle says plainly that it can't know whether the effect happened.
- **Chaos tests** (§52) kill Noodle at random points during writes, thousands of times, and prove that recovery always produces a valid world at the same or an earlier step — never a corrupt one.
- If Noodle itself hits an internal error, it saves the world when it safely can, writes a crash report, and shows a friendly message (§15).

### §41 — Security and capabilities

Programs are safe by default (Prime Directive 8). A program gets no file, network, process, environment, or device access unless it is granted — in plain English, in `project.noodle`:

```noodle
this program may read files in "data"
this program may write files in "output"
this program may talk to "api.example.com"
```

- A missing permission stops the program with an explanation: what it tried, on which line, the exact line to add to `project.noodle`, and the risk in plain words. In the REPL, Noodle MAY ask "allow this once?" and record the answer in the journal.
- Every check happens in the one effect gateway (§22): one door, one check. Paths can never escape granted folders through `..` or symbolic links.
- Recipes (§59) get no capabilities unless a project grants them to that recipe by name.
- **No foreign function interface.** Noodle programs can't call code written in other languages. That is a safety rule and a purity rule.
- **Never invent cryptography.** Implement only published, standard algorithms, exactly as specified and verified against official test vectors, and have security-critical code reviewed by experts before relying on it.
- Run records, saved worlds, and recipes from other people are untrusted input. Validate everything on load. Replay never performs irreversible effects (§22).
- `SECURITY.md` holds the threat model, the supported versions, and how to report a vulnerability.

### §42 — Resource limits

- Limits exist for steps, recursion depth, memory, history size, output size, file sizes, and the number of branches and checkpoints.
- Defaults are generous for learning but always finite. Projects change them in plain English (candidate: `this program may use up to 2 gigabytes of memory`).
- Limits count steps and bytes rather than wall-clock time wherever possible, so replay reproduces them exactly.
- Reaching a limit is never a crash. Noodle checkpoints the world and explains: *"Your loop on line 7 has run 10 million times. Here is how `count` changed in the last rounds. Is it stuck?"*

### §43 — Files and network

**Files**

- Text and bytes; UTF-8 by default; paths relative to the project.
- Reading a file is a recorded input. Writing inside a granted folder is a reversible output (§22).
- Friendly failures: a missing file lists similar names; a permission problem explains the grant it needs (§41).
- Candidates: `make lines is the lines of file "data/scores.txt"` and `write report to file "output/report.txt"`.
- Reading and writing CSV and JSON *data* belongs in the standard library, written in Noodle. Configuration stays in Noodle syntax (§4).

**Network** (gate G10 — after the core is solid)

- Start small: HTTP requests to granted hosts only.
- Anything sent is an irreversible output; every response is a recorded input. Replay never touches the network.
- **TLS is an owner decision** (§74): either a Noodle implementation of standard TLS — verified against official test vectors, tested for interoperability, and reviewed by experts — or the operating system's own TLS service through the OS door, listed in `PURITY.md`. Until one of those exists, Noodle says plainly that HTTPS isn't available.

### §44 — Concurrency

- The core language is single-threaded and deterministic. Threads, locks, and shared mutable state are never exposed to programs.
- Concurrency arrives only as worlds that exchange messages in a deterministic, recorded order (§13 #27) — after SELF HOSTING ACHIEVED, through a NEP, and with owner approval.
- The compiler MAY use parallelism internally later, but only if its output stays bit-identical (§51).

### §45 — Performance

- Correct first. Then measure. Then optimize only what measurements show matters.
- Benchmarks are Noodle programs in `benchmarks/`: compiling the compiler, running the examples, the cost of checkpoints, merges, and replay, history growth, and the overhead of each recording level.
- `PERFORMANCE.md` records budgets set from real measurements — for example, how long self-compilation may take on a reference machine, and the maximum overhead of `light` recording.
- CI tracks benchmark trends. On dedicated or pinned machines, it fails on regressions beyond a stated tolerance, using repeated runs to control noise.
- Never write "fast" without a benchmark behind it (§6).

### §46 — The standard library ("the pantry," working name)

- Written in Noodle. It never wraps another language's library.
- Always available, since Noodle has no imports. Decide by NEP how pantry names and user names coexist so that nothing shadows silently (§14) — for example, phrase pantry teachings as multi-word English, and make a clashing user definition an error that suggests a new name.
- Contents, growing only by NEP: Unicode-correct text; list and table helpers; exact math (fractions, decimals, rounding, powers, and roots with documented precision); recorded randomness; recorded dates and times; files (§43); CSV and JSON data; hashing (SHA-256); command-line arguments; terminal helpers such as color, where supported; testing helpers.
- Data tables such as Unicode properties and time zones are generated from official sources by Noodle tools — never by scripts in other languages.
- Every pantry teaching carries examples that are also tests (§13 #23).
- Keep the pantry small. Bigger things become recipes (§59).

### §47 — Platforms

- Tier-1 platforms, in order: Linux x86-64, then Linux AArch64, macOS AArch64, and Windows x86-64 — plus WebAssembly for the playground. Any other target needs an owner decision.
- Every Tier-1 platform passes 100% of the conformance suite with identical results (Law 6), and its OS door is documented in `PURITY.md`.
- Minimum operating-system versions are documented.
- **Clean-machine test:** for every Tier-1 platform, a fresh machine with nothing else installed downloads the release and passes the full suite.

---

## PART V — SELF-HOSTING

### §48 — The self-hosting plan

- The compiler, World VM, runtime, standard library, and tools are written in Noodle from the start (§5). Until SELF HOSTING ACHIEVED, anything the bootstrap must run stays inside the Seed subset — the bootstrap has to be able to run the compiler (§49, step 1). After that, the N−1 rule decides which features the compiler may use (§5).
- Write `SELF_HOSTING.md` early and keep it current: every component and its source files; which Noodle features each one uses; the **expressiveness gap analysis** — everything a compiler needs (records, kinds, `choose`, tables, text, bytes and bits, file input and output, recursion, failure handling) and whether Seed provides it comfortably; the plan; and progress, with evidence.
- Measure how long the bootstrap takes to run the compiler. Slow is fine. Unknown is not.
- **Dogfood everything.** Noodle's tools are Noodle programs, and developing Noodle uses Noodle's own world features: replay compiler bugs, ask `why` about the compiler's values, checkpoint long builds.
- Keep the compiler's source readable. It is the largest Noodle program in existence, and a teaching example in its own right.

### §49 — The proof: SELF HOSTING ACHIEVED

`noodle selfhost verify` — written in Noodle — performs every step below and records the full log:

1. The bootstrap runs the Noodle compiler's source, which compiles that same source into a native executable: **compiler₁**.
2. compiler₁ compiles the compiler's source: **compiler₂**.
3. compiler₂ compiles the compiler's source: **compiler₃**.
4. **Fixpoint:** compiler₂ and compiler₃ are bit-for-bit identical. Because compiler₁ comes from the same compiler logic running on the bootstrap, compiler₁ ≡ compiler₂ is expected too; any difference reveals a disagreement between the bootstrap and native code, and must be explained before the claim is made.
5. compiler₂ passes the entire test and conformance suite on both engines: the World VM and native code.
6. On a clean machine with no other programming language, compiler, interpreter, or bootstrap installed, compiler₂ rebuilds itself and every tool from source, reaches the same fixpoint, and passes the suites.
7. The hashes of compiler₁, compiler₂, and compiler₃, the source revision, and the full log are published in `SELF_HOSTING.md` and the release notes.
8. For later releases — **Diverse Double-Compiling:** build the compiler through independent paths — the previous release, a chain that starts from the frozen bootstrap, and eventually the from-nothing chain (§5) — and confirm the results are bit-identical.

Only when steps 1–7 pass may Noodle print:

```text
SELF HOSTING ACHIEVED
compiler₂ ≡ compiler₃ · sha256 <hash>
All suites passed on the World VM and native code.
Rebuilt on a clean machine with no other language installed.
```

Anything short of this is "self-hosting in progress," and every report says so.

### §50 — The Purity Ladder

| Stage | Name | What is true | Gate |
|---|---|---|---|
| 0 | External bootstrap | The tiny bootstrap runs Noodle Seed programs | G1 |
| 1 | Noodle reads Noodle | The lexer and parser, written in Noodle, parse every example and their own source | G2 |
| 2 | Noodle runs Noodle | The World VM, written in Noodle, runs Noodle programs — on the bootstrap | G3 |
| 3 | Native and self-contained | Noodle's own backend emits executables that need nothing else | G7 |
| 4 | Noodle builds Noodle | SELF HOSTING ACHIEVED (§49) | G8 |
| 5 | Bootstrap retired | Normal development and CI never use the bootstrap; it is frozen for audits | G9 |
| 6 | Fully self-hosted development | Every tool used to develop, test, document, and release Noodle is Noodle | G9 |
| 7 | Built from nothing | A hand-auditable seed builds today's Noodle with no other language in the chain (§5) | G11 |

Every report and every release states the current stage. Stages are climbed in order and never claimed early.

### §51 — Reproducible builds

- The same source, built by the same compiler release, produces bit-identical outputs on every host, for every target.
- Nothing may leak into a build: no timestamps, random identifiers, absolute paths, usernames, host names, locale, environment variables, file-listing order, hash-table order, or parallel-scheduling order.
- `noodle build --verify` builds twice and compares. CI also builds on two different host platforms and compares.
- Every release publishes its hashes, so anyone can rebuild it and compare. Learn from the reproducible-builds community's practices.
- The compiler that builds a release is itself a pinned earlier release (the N−1 rule, §5).

---

## PART VI — QUALITY

### §52 — Testing

Every test runner is written in Noodle, and every test is a Noodle program or Noodle data. Until the real test runner exists (gate G4), the bootstrap's minimal test mode fills the gap (§5).

- **Unit tests** for every part of the compiler, runtime, and tools — as `test` blocks (§13 #22) once they exist.
- **Lexer and parser tests,** including round trips: parse, format, and parse again to get the same tree.
- **Golden error tests:** the exact text of every error in the catalogue (§15), including "did you mean?" suggestions.
- **Runtime tests** for the meaning of every construct.
- **World tests** for history, provenance, checkpoints, branches, merge, experiments, rewind, replay, observers, and protections.
- **Property tests** for the World Laws (§33), with random programs, inputs, checkpoints, and rewind points.
- **Fuzzing** of the lexer, parser, compiler, and every file loader. The only acceptable outcomes are a correct result or a friendly error — never a crash, a hang, or a stack trace.
- **Differential tests:** the World VM against native code (Law 9); the current release against the previous one; provenance against the simple reference slicer (§24).
- **Metamorphic tests:** changes that must not change meaning — renaming, reformatting, adding comments, changing the recording level.
- **Mutation tests:** deliberately break the compiler and runtime, and confirm the tests notice. Track the score.
- **Chaos tests** for crash recovery (§40).
- **Security tests:** capability escapes, path tricks, and malicious run files, saved worlds, and recipes.
- **Performance tests** against the budgets in `PERFORMANCE.md` (§45).
- **Regression tests:** every fixed bug gets a test named after its issue.
- **Documentation tests:** every example in the docs and the beginner book runs and matches its shown output.

`TEST_STRATEGY.md` explains the strategy and reports coverage, measured by a Noodle tool.

### §53 — The conformance suite

- `conformance/` is the executable definition of Noodle. Each test is a program plus its exact expected output and exact expected errors — and, where relevant, its expected history, `why` answers, and journal.
- It is organized by specification section. Every rule in the specification links to its tests, and every test links back to its rule.
- It runs on every engine and platform: the bootstrap (for the Seed subset), the World VM, and native code on every Tier-1 platform. Shipping requires 100%.
- It includes every example that has ever worked — existing examples never break — and the North Star (`conformance/north_star/`, §7).
- It is versioned with the specification and pinned by edition (§63).

### §54 — The red team

Before every gate and every release, attack your own work. Record each question, what you found, and what you fixed in `REDTEAM.md`:

- Is any part of Noodle secretly implemented in another language? Does anything shell out to `as`, `ld`, `cc`, `git`, `python`, `node`, or any other tool?
- Is the bootstrap growing, or doing real work beyond running Seed?
- Is the history real, or reconstructed for display? Can `why` ever invent an answer? Test it with programs whose true cause is surprising.
- Can rewind silently lose data? Can replay diverge without saying so?
- Can an experiment leak into the main world? Can an irreversible effect ever happen inside an experiment or a replay?
- Can a program escape its capabilities through paths, symbolic links, recipes, or malformed files?
- Can any input make Noodle crash, hang, or show a stack trace?
- Would a beginner understand every error? Pick ten at random and try them on someone.
- Does the fixpoint hold on a truly clean machine, or does it lean on something already installed?
- Do the tests test real behavior? What is the mutation score?
- Does any document claim more than the tests prove?
- What could be removed without losing anything?

---

## PART VII — TOOLING

### §55 — The command line

One executable, `noodle`, written in Noodle. Before gate G7, `noodle` is the command line's Noodle source running on the bootstrap through a thin launcher that belongs to the bootstrap and counts against its cap.

```text
noodle new bank                  create a project with a first program and a first test
noodle run bank.noodle           run a program in its world (keeps a run record)
noodle check bank.noodle         find problems without running
noodle test                      run tests, examples-as-tests, and properties
noodle repl                      an interactive, live world
noodle build                     make a native executable (--target to cross-compile)
noodle format                    format code the one standard way
noodle lint                      gentle style advice, never required
noodle explain N0042             turn an error code into a mini-lesson
noodle debug bank.noodle         the time-travel debugger
noodle replay bank.run           replay a recorded run (--with-current-code --compare)
noodle why bank.run balance      ask why about a recorded run
noodle timeline bank.run         show the timeline of a run
noodle story bank.run            narrate a run in plain English
noodle resume bank.world         continue a saved world
noodle inspect bank.world        a readable view of any stored file
noodle profile bank.noodle       where time and memory go
noodle bench                     run the benchmarks
noodle purity                    the purity audit and Purity Score
noodle selfhost verify           the self-hosting proof (§49)
noodle disassemble app           show generated machine code
noodle docs                      build and open the documentation
noodle lsp                       the language server for editors
noodle ci                        everything CI runs (§65)
noodle version                   version, edition, build hash, and ladder stage
noodle help                      friendly help for everything
```

Later, each through a NEP: `noodle recipe`, `noodle translate`, `noodle explore`.

- `noodle` on its own prints a short, friendly guide. A single `.noodle` file runs without a project — beginners never need setup.
- Every message follows the error style (§15). Exit codes are documented. Tools that need machine-readable output get a documented data format.

### §56 — The REPL

- Understands open blocks (`if … end`) and waits for the rest. Line editing and input history are written in Noodle.
- An error never loses your work: the world stays exactly as it was before the failing line.
- The REPL is a live world: `show state`, `show history`, `why`, `checkpoint`, `rewind`, `undo`, `step back`, `branch`, `experiment`, `what if`, and `save the world` all work. `undo` returns to the automatic checkpoint taken before the last line.
- **Save what I did:** write the session out as a `.noodle` program containing only the lines that stuck — undone lines left out.
- Works in any terminal, and degrades gracefully without color.

### §57 — Formatter, linter, debugger, and editor support

- **Formatter:** one canonical style, idempotent, and meaning-preserving (proved by metamorphic tests, §52). The specification decides whether it normalizes keyword case.
- **Linter:** gentle suggestions, never a gate for beginners.
- **Documentation generator:** builds the reference, the guides, and the beginner book from Markdown and Noodle examples, running every example as a test.
- **Time-travel debugger,** built on the world model rather than on operating-system debugging hooks: break on a line or on a condition (*stop when money is below 0*), step forward and back, go to a step, go to when a value changed, and ask `why` at any point.
- **Language server:** `noodle lsp`, written in Noodle and speaking the Language Server Protocol — errors as you type, hovers that show a value's history and `why` from the last run, go to definition, rename, and format.
- **Editor extensions** are thin host shims (§4): they start `noodle lsp` and contain no language logic. Syntax-highlighting grammars are data files. All are listed in `PURITY.md` with their sizes.

### §58 — The playground

- A web page where anyone can write and run Noodle with nothing installed.
- The compiler and World VM run in the browser as WebAssembly produced by Noodle's own backend.
- The browser side is a tiny loader shim that starts the module and passes text in and out. Everything else — including drawing timelines and branches — is Noodle compiled to WebAssembly. **The browser must never become the language.**
- It shows the world visually — timeline, history, `why`, branches — and runs every example.
- Programs can be shared as links that carry the code itself, so no server is required.
- It runs inside the browser's sandbox, with no files or network by default.

### §59 — Recipes (later)

- Recipes are shared Noodle code: folders of Noodle source, copied into a project's `recipes/` folder and pinned by content hash in `recipes.lock.noodle` (§14).
- No install scripts, no build steps, no foreign code, and no capabilities unless granted by name (§41).
- Managed with `noodle recipe add`, `remove`, `update`, and `verify`. A public registry is optional and needs an owner decision.
- Recipes arrive at gate G10, after the language and its formats are stable.

### §60 — Examples

Every example lives in `examples/`, runs in CI against its exact expected output, and appears in the docs. At minimum:

**Basics**

1. `hello.noodle` — `say hello`
2. `money.noodle` — `make`, `if`, `otherwise` (the example in §9)
3. `ask_name.noodle` — `ask` and `say`
4. `calculator.noodle` — arithmetic and friendly errors
5. `countdown.noodle` — `repeat`
6. `shopping_list.noodle` — lists, `add`, `for each`
7. `teach.noodle` — `teach`, `do`, `give back`
8. `guessing_game.noodle` — recorded randomness, loops, validated `ask`
9. `grades.noodle` — records, lists, averages
10. `todo.noodle` — `remember` across runs

**World features**

11. `history.noodle` — `show history` and questions about time
12. `why.noodle` — `why is profit 500`
13. `checkpoint_rewind.noodle` — checkpoints, rewind, and the abandoned branch
14. `branches.noodle` — branch, compare, merge, and a merge conflict
15. `experiments.noodle` — experiment, keep, discard, and `what if`
16. `watch_protect.noodle` — `watch`, `whenever`, and `protect`
17. `replay_fix.noodle` — replay with fixed code
18. `save_resume.noodle` — save the world and resume it

**Real programs**

19. `text_adventure.noodle` — kinds, `choose`, tables, save and resume
20. `data_report.noodle` — read a CSV file (with a grant), filter it, write a report
21. `simulation.noodle` — a small world simulation, explored with experiments
22. `bank.noodle` — the North Star (§7)

Every example already in the specification or the repository is kept and becomes a conformance test. The ultimate example is Noodle itself.

### §61 — Documentation

Documentation is written together with the code, and states plainly what is real and what is planned.

- `README.md` — what Noodle is, how to install it, a first program, the Purity Score, and the ladder stage.
- `MISSION.md` — this document.
- `spec/SPEC.md`, `spec/SEED.md`, and the error catalogue.
- `ARCHITECTURE.md`, `BOOTSTRAP.md`, `BOOTSTRAP_DEPENDENCIES.md`, `SELF_HOSTING.md`, `PURITY.md`, and — late — `BUILD_FROM_NOTHING.md`.
- `ROADMAP.md`, `STATUS.md`, `HANDOFF.md`, `DECISIONS/` (decision records), and `neps/`.
- `CHANGELOG.md`, `CONTRIBUTING.md`, `CODE_OF_CONDUCT.md`, `SECURITY.md`, and `LICENSE`.
- `TEST_STRATEGY.md`, `PERFORMANCE.md`, `REDTEAM.md`, and `BEGINNER_LOG.md`.
- `docs/WORLD.md` (the world model), `docs/EFFECTS.md`, `docs/REPLAY.md`, `docs/ERRORS.md`, `docs/FORMATS.md`, `docs/IR.md`, and `docs/MACHINE_LAYER.md`.
- `docs/reference/` — the language reference.
- `docs/guides/` — how-to guides.
- The beginner book, **"Learn Noodle in an Afternoon,"** tested with real beginners (§16).

---

## PART VIII — DISCIPLINE

### §62 — Repository layout

```text
(repository root)
├── MISSION.md
├── README.md
├── project.noodle        Noodle's own project file, in Noodle syntax
├── bootstrap/            temporary, capped, retired at G9 (§5)
├── shims/                tiny, logic-free host loaders (§4)
├── spec/                 SPEC.md, SEED.md, the error catalogue
├── neps/                 Noodle Enhancement Proposals
├── noodle/               Noodle's own source
│   ├── compiler/         lexer, parser, name resolution, checks, IR
│   ├── backend/          code generation, assembler, linker, disassembler
│   ├── machine/          Machine Layer files only (§35)
│   ├── runtime/          values, memory, the effect gateway
│   ├── world/            World VM, history, provenance, checkpoints, branches, replay
│   ├── storage/          formats, journals, crash recovery
│   ├── pantry/           the standard library
│   └── tools/            cli, repl, formatter, lsp, debugger, test runner, purity, selfhost, ci
├── selfhost/             fixpoint logs and published hashes
├── conformance/          the executable definition of Noodle (including north_star/)
├── tests/
├── examples/
├── benchmarks/
├── docs/
├── DECISIONS/            decision records
└── releases/             release notes and hash lists (Noodle syntax)
```

The other root documents from §61 sit beside `README.md`. If a repository already exists with a different layout, move toward this one gradually, with a decision record, and without losing history.

### §63 — Versions and editions

- The toolchain uses semantic versioning: MAJOR.MINOR.PATCH.
- The **language** changes through **editions** (candidate: `this project uses edition 2027` in `project.noodle`). Changes that break existing code happen only in a new edition. Old editions keep working, and a migration tool rewrites code for a new edition (later).
- The specification, the conformance suite, and every file format carry their own versions.
- The promise, stated in the README: **a program that runs today keeps running.**

### §64 — Git discipline

- Small commits. Each one keeps everything green and says what changed, why, and where the evidence is:

```text
World: keep rewound-away steps as an abandoned branch

Rewind no longer discards C and D; they become branch abandoned-1.
Tests: conformance/world/rewind_*.noodle (9 passing).
Law 2 property test: 10,000 random runs, no failures.
```

```text
Parser: show both readings of mixed and/or lines (N0107)
Bootstrap: remove an unused list helper (2,410 of 3,000 lines)
```

- Never force-push the main branch. Tag every release.
- Git is for version control only. Building, testing, and releasing Noodle never require Git: a plain source archive builds.
- Never commit secrets, large binaries, or generated files — except published release artifacts and their hashes.

### §65 — Continuous integration

- The CI host's configuration contains one command: `noodle ci` (§4). The only temporary addition belongs to the bootstrap's own exception (§5): until G9, CI may first build the bootstrap exactly as `BOOTSTRAP.md` documents. That step disappears with the bootstrap.
- `noodle ci` runs the purity audit, the format check, the build, every test category (§52) on every engine, the conformance suite, all examples and documentation tests, the reproducibility check (§51), `noodle selfhost verify` (from G8 on), the security tests, and benchmark tracking — on every Tier-1 platform (§47) that has been reached.
- A skipped test needs a linked issue and an expiry date. An expired skip fails CI.
- Red CI blocks merging. Never "fix" CI by disabling a check.

### §66 — Releases

Every release includes:

- the version and edition, with release notes that claim nothing without evidence (§6);
- executables for every Tier-1 platform, each with its hash;
- the Purity Score, the ladder stage (§50), and — until it is retired — the bootstrap's size;
- conformance results for every engine and platform;
- known limitations, stated plainly;
- instructions for rebuilding the release and comparing hashes (§51).

From G8 on, every release is built by the previous release (the N−1 rule, §5). A Noodle tool assembles releases deterministically. Pre-releases are labeled experimental.

### §67 — Research

Before designing each area, study its prior art. Record what you took, what you rejected, and why, in `DECISIONS/`. Verify every claim before relying on it.

- **Bootstrapping and self-hosting:** Go's move from C to Go (Go 1.5); Rust's first compiler, written in OCaml; OCaml's bootstrap compiler kept in its own repository; Zig's WebAssembly bootstrap seed; GCC's three-stage bootstrap; the bootstrappable-builds work (stage0, GNU Mes); Ken Thompson's "Reflections on Trusting Trust"; David A. Wheeler's Diverse Double-Compiling.
- **Systems written in themselves:** the Go runtime, Oberon, Jikes RVM, Maxine, RPython and PyPy.
- **Time travel and provenance:** rr (record and replay), omniscient debugging, event sourcing, as-of queries in temporal databases, dynamic program slicing, data provenance research.
- **Incremental computation:** Adapton, Salsa.
- **Content addressing:** Git's object model; Unison's content-addressed code.
- **Persistent data structures:** hash array mapped tries, RRB-trees, Clojure's collections.
- **Memory management:** Lean 4's "Counting Immutable Beans"; Koka's Perceus.
- **Beginner-friendly languages:** Quorum, Hedy, Scratch, Logo, BASIC, HyperTalk, Inform 7, AppleScript. Learn above all from English-like languages that turned out *easy to read but hard to write*. Noodle must be easy to read **and** easy to write.
- **Friendly errors:** the error-message design of Elm and Rust.
- **Security:** object-capability systems and WASI's capability model.
- **Reproducible builds:** the reproducible-builds community's practices.

### §68 — Originality

- Design Noodle's own solutions. Learn from others, never copy code or text you have no right to use, and respect every license.
- Never claim "first," "only," or "revolutionary" unless a documented prior-art search establishes it (§6). Describe what Noodle does, and let people judge.
- Credit influences openly in the docs.

### §69 — Governance and license

- The license is the owner's decision (§74). Propose one with reasons — for example, Apache-2.0 (which includes a patent grant) or MIT.
- Before any public launch, search for existing languages, projects, and trademarks named "Noodle," report what you find to the owner, and propose alternatives if needed.
- `CONTRIBUTING.md` covers the purity rules, the NEP process, how decisions are made, and a developer certificate of origin or contributor agreement (the owner's choice). Until the owner sets a governance model, the owner approves every NEP.

### §70 — Accessibility and internationalization

- Every tool works with a screen reader and a keyboard alone. No information is carried by color alone.
- The playground and the docs follow the Web Content Accessibility Guidelines.
- All user-facing text lives in the message catalogue (§15), ready for translation. Language packs come later (§13 #30).
- Unicode names and text work everywhere, including right-to-left scripts where the terminal allows.
- The docs use plain language: short sentences, defined terms, no unexplained jargon.

---

## PART IX — EXECUTION

### §71 — The vertical method

Build thin, complete slices, not isolated layers. The first slice is `say hello`, running end to end. For every feature after that:

1. Read its section of the specification — or draft one (§8).
2. Write or update its NEP if the feature is new (§17).
3. Write the conformance tests first: exact output, exact error text.
4. Lexer.
5. Parser and syntax tree.
6. Name resolution and meaning checks.
7. IR.
8. World VM — including what the feature records in history and provenance.
9. Friendly errors, added to the catalogue with golden tests.
10. Native backend (from G7 on).
11. Docs and an example.
12. Run everything, commit, and update `STATUS.md`.

Never build a complete parser before anything runs. Never end a session with the tree red.

### §72 — The gates

Gates are passed in order. A gate is passed only when every criterion is proven, with the evidence recorded in `STATUS.md`. Never start a gate while tests are red. The owner may reorder gates with a decision record.

| Gate | Name | Proven when | Ladder |
|---|---|---|---|
| G0 | Orientation | The first task (§76) is complete | — |
| G1 | Seed | `spec/SEED.md` is approved; the bootstrap, under its cap, runs Seed programs; its minimal test mode works; `say hello` runs; the first `noodle purity` — itself a Seed program — reports honestly | 0 |
| G2 | Noodle reads Noodle | The lexer, parser, syntax tree, and friendly syntax errors — written in Noodle, running on the bootstrap — handle every example and their own source; round-trip tests pass | 1 |
| G3 | Noodle runs Noodle | The World VM, written in Noodle, runs every core feature (§11) through the effect gateway (§22); every run produces a journal; golden error tests and basic limits (§42) pass | 2 |
| G4 | Real programs | Tier 1 (§13 #1–#12) and Tier 3 #22–#25 work; files work with grants (§41, §43); the Noodle test runner replaces the bootstrap's test mode; the gap analysis (§48) is closed | — |
| G5 | A world that remembers | Transitions, history, provenance, `why`, `show`, `remember`, `watch`, `whenever`, and protections (including #26) work; Laws 1, 7, and 8 pass | — |
| G6 | A world that travels | Checkpoints, branches, compare and merge, experiments, `what if`, rewind, replay, questions about time, undo and stepping, replay with fixed code, and save and resume work; Laws 1–8 pass; North Star steps 1–7 pass on the World VM | — |
| G7 | Native | Noodle's own backend, assembler, and linker produce Linux x86-64 executables; the World VM and native code agree on the whole conformance suite (Law 9); outputs are reproducible | 3 |
| G8 | SELF HOSTING ACHIEVED | §49 steps 1–7 pass and are published; Law 10 holds; the whole North Star passes on native code | 4 |
| G9 | Bootstrap retired | Development and CI never use the bootstrap; every development tool is Noodle; the N−1 rule is in force | 5–6 |
| G10 | Ecosystem | More platforms (§47), the playground, the language server and editor shims, network access with grants (§43), recipes, a growing pantry, the beginner book — and Tier 4 features only through NEPs with owner approval | — |
| G11 | From nothing | The build-from-nothing chain (§5) is documented and reproducible, and Diverse Double-Compiling (§49, step 8) passes | 7 |

### §73 — Agent operating protocol

**At the start of every session**

- Re-read Part I, `STATUS.md`, `HANDOFF.md`, and the recent decision records.
- Run the full test suite. If anything is red, fixing it comes first.
- State the plan: the next smallest vertical slice.

**During the session**

- Work in small steps. Commit only when green.
- Record every significant decision in `DECISIONS/NNNN-title.md`: context, options, decision, consequences.
- Update documentation together with the code.
- Never skip, weaken, or delete a failing test to get green (§6).
- If you are blocked on an owner decision (§74), write down the question, choose a reversible default where one exists, record it, and move on to work that doesn't depend on the answer.
- Estimate honestly. If something will take weeks, say so.

**At the end of every session**

- Update `STATUS.md` (what works, with evidence) and `HANDOFF.md` (exactly where you stopped, the next step, and anything surprising).
- Report in this format:

```text
DONE:          what was finished, each item with evidence (test names, command output)
NOT DONE:      what was attempted but not finished, and why
DECISIONS:     decisions made, with links to DECISIONS/
QUESTIONS:     questions for the owner (§74)
NEXT:          the next smallest step
PURITY SCORE:  n% · bootstrap x of 3,000 lines · shims y
LADDER:        stage n — name
GATE:          Gn — name · criteria proven: m of k
```

### §74 — Decisions reserved for the owner

Ask before doing any of these, and offer a recommendation with reasons:

1. Changing any existing syntax or meaning in the specification.
2. Choosing the bootstrap's language, or changing its size cap.
3. Adding or growing any host shim.
4. Adding any OS door beyond §4 — for example, the operating system's TLS service (§43).
5. Choosing the license, or renaming the project (§69).
6. Making the repository public, or publishing a release.
7. Starting any Tier 4 feature (§13), or growing the keyword budget beyond the approved plan (§17).
8. Default capabilities (§41) and default resource limits (§42).
9. Edition breaks and removals (§63).
10. Anything irreversible: deleting history, rewriting a published release, force-pushing the main branch.

While you wait for an answer, keep working on anything that doesn't depend on it.

### §75 — Definition of done

Noodle 1.0 is done when every item below is proven with evidence:

- [ ] Every original core feature (§11) and every original advanced capability — `remember`, `show history`, `why`, `checkpoint`, `branch`, `experiment`, `rewind`, `replay`, `compare`, `watch`, `protect`, `show state` — works with real semantics.
- [ ] Tiers 1–3 (§13) are implemented, each with a NEP, spec text, tests, docs, an example, and friendly errors.
- [ ] Every error in the catalogue has a golden test, and fuzzing shows that no user ever sees a crash or a stack trace.
- [ ] All ten World Laws (§33) hold, on every engine.
- [ ] The North Star (§7) runs end to end on native code.
- [ ] SELF HOSTING ACHIEVED (§49): hashes published, fixpoint reached on a clean machine.
- [ ] The bootstrap is retired, the Purity Score is 100% outside host shims, and nothing but Noodle is needed to build, test, or release Noodle.
- [ ] Builds are reproducible across every Tier-1 platform (§51).
- [ ] The test suite covers every category in §52, and conformance is 100% on every engine and platform.
- [ ] The command line, REPL, formatter, debugger, language server, and playground work (§55–§58).
- [ ] The documentation (§61) is complete and honest, every example runs, and the beginner book has been tested with real beginners.
- [ ] A beginner can install Noodle and run a first program within minutes.
- [ ] The security model is implemented and red-teamed (§41, §54).
- [ ] The performance budgets are met (§45).
- [ ] A release is published with notes, hashes, the Purity Score, and the ladder stage (§66).

### §76 — Your first task

Do not start writing the compiler yet. First:

1. Save this document in the repository as `MISSION.md`.
2. Read the Noodle specification completely. If there isn't one, draft `spec/SPEC.md` from this document, with every syntax decision marked PROPOSED (§8).
3. Inventory the existing repository, if there is one: what exists, what runs, which language each part is written in, and what must change to meet §4. Run whatever exists and record the results honestly. Don't delete existing non-Noodle code blindly: keep it as a reference until Noodle equivalents pass the same tests, then remove it and record the removal in `BOOTSTRAP_DEPENDENCIES.md` and `PURITY.md`. If an existing implementation could be trimmed into the bootstrap under its cap, propose that as one option (§74).
4. Write `ARCHITECTURE.md`, `BOOTSTRAP.md` (with your proposed bootstrap language and size cap), `BOOTSTRAP_DEPENDENCIES.md`, `ROADMAP.md` (organized by the gates of §72), `TEST_STRATEGY.md`, `PURITY.md` (the first honest audit), and `STATUS.md`.
5. Draft `spec/SEED.md`: the proposed Seed subset.
6. List every question for the owner (§74), each with your recommended answer.
7. Report in the format of §73. Then begin G1.

### §77 — The final principle

```text
                   SIMPLE ON THE SURFACE
                            │
              POWERFUL WORLD MODEL UNDERNEATH
                            │
  REMEMBER · WHY · CHECKPOINT · BRANCH · EXPERIMENT · WHAT IF
          MERGE · REWIND · REPLAY · WATCH · PROTECT
                            │
           PURE · SELF-HOSTED · REPRODUCIBLE
                            │
                 NOODLE, BUILT BY NOODLE
```

- Do not sacrifice beginner simplicity for implementation convenience.
- Do not sacrifice correctness for demos.
- Do not sacrifice honesty for impressive claims.
- Do not sacrifice Noodle's identity to imitate other languages.
- Do not sacrifice purity for shortcuts.

### §78 — The final mission

Build NOODLE into a real programming language: simple enough for a beginner's first afternoon, powerful enough to build itself, honest enough that every claim has evidence, pure enough that nothing but Noodle is needed to build it — and different, because every program runs in a world that remembers what happened, explains why, and can safely travel through time.

Start with the first task (§76). Build vertically. Prove everything.

Every decision answers to one question:

> **Can Noodle build Noodle — from its own source, with its own compiler, into the exact same bytes, with nothing else installed?**

**END OF MASTER PROMPT**
