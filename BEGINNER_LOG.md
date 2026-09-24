# Beginner log

Every observed confusion, and what changed because of it (§16).

**No real beginners have been tested yet.** The entries below come from documented heuristics: the agent wrote programs the way a beginner plausibly would and recorded where Noodle got in the way. Real sessions with five or more beginners are required before any keyword is final.

| Date | Source | Observation | Change |
|---|---|---|---|
| 2026-09-24 | heuristic | `make a is 5` is a very natural first line, but `a` was reserved | `a`/`an` made usable as names (DECISIONS/0004) |
| 2026-09-24 | heuristic | `factorial n minus 1` reads as factorial(n−1) to a mathematician, and as factorial(n)−1 to the parser | now an error that shows both readings. **Watch this:** it may frustrate beginners, see Q12 |
| 2026-09-24 | heuristic | `say you have money`: beginners will expect the value of `money` to be filled in | error that suggests `say "you have [money]"` |
| 2026-09-24 | heuristic | `for each item in list`: people name things `item` and `list` | both stay usable as names |
