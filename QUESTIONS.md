# Questions for the owner

Each question is reserved for the owner (§74). Each has a recommendation and the reversible default I'm using meanwhile, so work isn't blocked.

| # | Question | §74 | Recommendation | Default in use |
|---|---|---|---|---|
| Q1 | **Bootstrap language?** | 2 | **C99**, one file. It's available everywhere, easy to audit, and fits the from-nothing chain (stage0 → M2-Planet → Mes → TinyCC). See BOOTSTRAP.md and DECISIONS/0001. | C99 |
| Q2 | **Bootstrap size cap?** | 2 | **3,000 lines** for everything in `bootstrap/`, as §5 suggests. Today's core uses 1,137. I expect Seed Part B to need about 1,000–1,500 more. | 3,000 |
| Q3 | **Approve `spec/SEED.md`** (Part A as built, Part B as proposed)? | 1 | Approve Part A. Approve Part B's direction, with each item still getting its own NEP. | building Part A, not Part B yet |
| Q4 | **License?** | 5 | **Apache-2.0.** It's already in the repository, and it includes a patent grant. | Apache-2.0 (existing) |
| Q5 | **Project name.** A search for other languages, projects, and trademarks named "Noodle" is required before any public launch (§69). | 5 | Let me run that search before G10 and report back. No rename now. | "Noodle" |
| Q6 | **Comments:** `#` to the end of the line? | 1 | Yes. It's short and familiar, and it doesn't clash with anything. | `#` |
| Q7 | **`ask` stores the reply in a name called `answer`** (Scratch style, and it matches §13 #8's `the number in answer`)? The alternative is `ask "…" into name`. | 1 | Keep `answer`. Add `into` only if beginners stumble. | `answer` |
| Q8 | **Counting loops:** `for each n from 1 to 10` instead of the §13 candidate `count from 1 to 10`? It reuses words instead of adding a keyword, and it names the counter. | 1, 7 | Yes. | `for each … from … to` |
| Q9 | **Teaching inputs joined by `and`:** `teach area width and height`, used as `area 3 and 4`. | 1 | Yes. Commas are saved for lists. | `and` |
| Q10 | **Names:** single words (`high_score`), case-insensitive. | 1 | Yes for now. Multi-word names can come later via a NEP. | as recommended |
| Q11 | **Showing fractions:** `1/3`, or a rounded decimal like `0.333…`? | 1 | `1/3`, because it's exact and honest. Terminating fractions show as decimals (`1.75`). | `1/3` |
| Q12 | **The two-readings rule:** `say double 4 plus 1` and `say length of word plus 1` are errors asking you to name the value first. That's strict, but it's what "never guess" means. The alternative is to document that phrases bind tightly, the way `(double 4) plus 1` would. | 1 | Keep it strict now. Collect beginner evidence. Relaxing it later is compatible, but tightening it later would break programs. | strict |
| Q13 | **Comparing different kinds** (`5 is "5"`) is an error instead of `no`. | 1 | Yes, because it's almost always a bug. | error |
| Q14 | **Default resource limits** for the World VM: recursion depth, steps, memory. | 8 | Recursion 10,000, steps 100 million per run, memory 1 GB. All can be changed in `project.noodle`. The bootstrap separately uses recursion 2,000 and no step limit. | none in VM yet |
| Q15 | **Default capabilities:** none (§41). | 8 | None, exactly as the mission says. | none |
| Q16 | **Mixing `and` with `or`:** is splitting into two lines enough for now, or should we design `both … and …` / `either … or …` next? | 1, 7 | Splitting is enough for Seed. Design `both/either` with beginner evidence in G4. | splitting |
| Q17 | **Pushing to GitHub CI:** add a CI config that builds the bootstrap and runs both suites under a timeout? | — | Yes. It's a small YAML file that runs the bootstrap build step allowed by §65. | not added yet |
