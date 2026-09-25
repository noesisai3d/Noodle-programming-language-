# 0005 — Files, folders, and arguments in Seed

**Status:** PROPOSED · owner approval needed (§74.1)
**Date:** 2026-09-25

## Context
The last G1 criterion that doesn't depend on the owner is a purity audit written in Noodle (§4, §72). To run it, Seed needs to read files and list folders. The compiler will need the same things later.

## Decisions (all PROPOSED)
| Form | Meaning | Where it comes from |
|---|---|---|
| `the text of file "p"` | the whole file as text | — |
| `the lines of file "p"` | a list of lines; a final line break doesn't add an empty line | §43 candidate |
| `write V to file "p"` | replace the file with V, shown as `say` would show it, without an added line break | §43 candidate |
| `the files in folder "p"` | every file under the folder, including subfolders and hidden files, as paths inside that folder, sorted by bytes | new |
| `the words of V` | V split at spaces, tabs, and line breaks | new |
| `the arguments` | the text after the program name on the command line | SEED Part B9 |
| `A starts with B`, `A ends with B` | yes/no, text only | §13 #6 candidate |

**Relative paths start from the folder of the program being run,** not from wherever the command was typed. That keeps a program's behavior the same no matter where it's started from. It's the single-file version of "paths relative to the project" (§43). A leading UTF-8 byte-order mark is dropped when a file is read.

## Consequences
- The bootstrap performs these effects with no permission check. Grants (§41) belong in the Noodle effect gateway, and the bootstrap is only a development tool.
- `the files in folder` sorts the whole list rather than folder by folder, so its order never depends on the operating system.
- Tests: `conformance/seed/files/*`, `text/words`, `text/starts_ends`, and five new golden errors.
