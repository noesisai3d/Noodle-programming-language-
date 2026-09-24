# Noodle Seed

**Status:** DRAFT, waiting for owner approval (gate G1 requires "`spec/SEED.md` is approved").
**Version:** 0.1.0-draft

Noodle Seed is the smallest strict subset of Noodle in which the Noodle compiler and the World VM can be written comfortably (§5). The temporary bootstrap (`bootstrap/seed.c`) interprets Seed and nothing else.

## The Seed promise

1. **Every Seed program is a valid Noodle program with identical behavior.** Seed never has its own meanings.
2. **Seed may refuse, but never differ.** Where the bootstrap can't do something the full language can, it stops with a friendly error. It never gives a different answer. The only such limit today is the **ASCII limit** below.
3. **Seed grows only when the compiler needs it,** and every addition is also a full Noodle feature with a spec section and tests.

## Part A: in Seed now (implemented in the bootstrap, with tests)

These are specified in [`SPEC.md`](SPEC.md) §1–§7 and tested in `conformance/seed/`.

| Feature | Forms | Tests |
|---|---|---|
| Comments | `# …` | `make/comments` |
| Showing | `say V`, bare words | `say/*` |
| Asking | `ask V` → `answer` | `ask/ask`, `examples/ask_name` |
| Names | `make N is V` | `make/*` |
| Exact numbers | literals, `plus minus times divided by`, `remainder of … divided by …`, `whole part of`, `the number in` | `numbers/*` |
| Text | `"…"`, `""`, `[slots]`, `[[`, `]]`, `length of`, `item N of`, `first/last item of`, `contains`, `for each` | `text/*`, `say/text_template` |
| yes/no | `yes no and or not`, the comparisons | `logic/*` |
| Lists | `a list of …`, `a new list`, `add … to …`, item access, `contains`, equality | `lists/*` |
| Choice | `if / otherwise if / otherwise / end` | `if/*` |
| Loops | `repeat N times`, `repeat while`, `repeat until`, `for each … in`, `for each … from … to`, `stop`, `skip` | `loops/*` |
| Teachings | `teach`, inputs joined with `and`, `give back`, `do`, recursion | `teach/*` |
| Friendly errors | 24 golden errors | `errors/*` |

**The ASCII limit.** The bootstrap has no Unicode tables. Operations that count or index characters (`length of`, `item N of`, `first/last item of`, and `for each` over text) work only on ASCII text in Seed. On other text the bootstrap stops with a friendly error. The full language counts user-perceived characters (§12). Text containing any Unicode can still be stored, compared, joined with slots, and shown.

## Part B: proposed additions the compiler needs (not implemented yet)

These are the gaps the gap analysis (§48) found. The compiler, World VM, and tools need each one. Syntax is PROPOSED and follows the §13 candidates unless a reason is given. Each gets a NEP before it lands.

| # | Feature | Proposed Seed form | Why the compiler needs it |
|---|---|---|---|
| B1 | Records (§13 #1) | `make tok with` / `kind is "word"` / `text is "say"` / `end`; `tok's kind`; `make tok's kind is …` | tokens, syntax-tree nodes, positions |
| B2 | Tables (§13 #2) | `a new table`, `put V in T under K`, `T under K`, `T has K` | symbol tables, keyword lookup |
| B3 | Kinds (§13 #3) | `a node is one of` / `a number_node with value` / … / `end`; `a number_node with value 5` | syntax trees, IR |
| B4 | choose / when (§13 #4) | `choose V` / `when it is a K` / `its field` / `otherwise` / `end` | walking trees |
| B5 | Failure handling (§13 #8) | `try` / `if that fails` / `the problem` / `end` | turning parse errors into catalogue errors |
| B6 | Files | `the text of file "p"`, `the bytes of file "p"`, `write V to file "p"` | reading source, writing executables |
| B7 | Bytes | a list of whole numbers 0–255; `text from bytes B`, `the bytes of T` (UTF-8) | UTF-8 decoding, machine code |
| B8 | Bit operations (§13 #12) | Not in Seed: written in Noodle with `remainder of`, `whole part of`, and `times`, then moved to the pantry | encoding instructions |
| B9 | Program arguments | `the arguments` (a list of text) | the command line |
| B10 | More text | `the code of C`, `the character with code N`, `starts with` | the lexer |
| B11 | Many files | Every `.noodle` file in a project folder shares one world of names (§14) | the compiler has many files |

**Permissions in the bootstrap.** Grants (§41) are enforced by the effect gateway, which is written in Noodle. The bootstrap is a trusted development tool. It performs B6 file operations only for development tools run by Noodle's developers, never for user programs. This is recorded in `BOOTSTRAP_DEPENDENCIES.md`.

## Part C: deliberately not in Seed

The world model (history, `why`, checkpoints, and so on), `remember`, randomness, time, `whenever`, `protect`, tests-in-code, properties, network, and every Tier 4 feature. These are implemented in Noodle, by the World VM, and the bootstrap never learns them (§5).
