# Error catalogue (draft)

**Status:** DRAFT. The catalogue moves into Noodle data files at G3 (§15). Codes (`N0001`, and so on) will be assigned then. Until then, each error is named after its golden test in `conformance/seed/errors/`, which holds the exact text.

Every error has the same shape (§15):

```text
Line <n>:
<plain explanation>

You wrote:
<the line>

<suggested fix>
```

**Known gap:** the marker under the exact spot on the line (§15) is not implemented yet.

| Name (golden test) | When | Phase |
|---|---|---|
| `missing_quote` | Text has no closing `"` | reading |
| `strange_symbol` | A symbol Noodle doesn't use, such as `+` | reading |
| `reserved_name` | A Noodle word used as a name | checking |
| `unknown_name` | A name that is never made (includes "Did you mean?") | checking |
| `bare_word_typo` | A bare word after `say` that is one or two letters away from a name | checking |
| `mixed_words_names` | Bare words mixed with names after `say` | checking |
| `mixed_and_or` | `and` and `or` on the same level of one line | checking |
| `ambiguous_call` | Arithmetic after a teaching's or phrase's value: two readings | checking |
| `shadowing` | A teaching's input or name matches a top-level name | checking |
| `missing_end` | A block with no `end` | checking |
| `stray_end` | An `end` that closes nothing | checking |
| `stop_outside_loop` | `stop` or `skip` outside a loop | checking |
| `teaching_without_do` | A line starting with a teaching's name | checking |
| `unknown_statement` | A line starting with an unknown word (includes "Did you mean?") | checking |
| `divide_by_zero` | Dividing by zero | running |
| `if_not_yes_no` | A condition that isn't yes/no | running |
| `compare_kinds` | `is` between different kinds of value | running |
| `text_plus` | Arithmetic on text | running |
| `too_deep` | Teachings nested past the depth limit | running |
| `nothing_given_back` | Using the result of a teaching that gives back nothing | running |
| `item_out_of_range` | `item N of` past either end | running |
| `not_a_number` | `the number in` on text that isn't a number | running |
| `add_to_non_list` | `add` to something that isn't a list | running |
| `used_before_made` | A name used before its first `make` runs | running |

The bootstrap also has these messages, which don't have golden tests yet: `this list is empty`, `needs a whole number`, `can only count plain English text` (Seed's ASCII limit, see SEED.md), `ran out of answers` (test mode), and `nothing to read`. They are tracked in TEST_STRATEGY.md.
