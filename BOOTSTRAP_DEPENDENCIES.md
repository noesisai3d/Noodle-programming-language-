# Bootstrap dependencies

Every external thing needed to build or run Noodle today. **This count only ever goes down** (§5).

**Count: 3**

| # | Dependency | Why it exists | Temporary? | How it will be removed | Status |
|---|---|---|---|---|---|
| 1 | A C99 compiler (gcc, clang, or tcc) | Builds `bootstrap/seed.c` | Yes | G8: compiler₁ is built by Noodle. G9: the bootstrap is retired. G11: the from-nothing chain replaces any installed compiler. | in use |
| 2 | The C standard library (libc) | stdio, stdlib, string, setjmp for the bootstrap | Yes | Goes away with the bootstrap. Native Noodle uses direct system calls on Linux (§36). | in use |
| 3 | POSIX `dirent.h` and `sys/stat.h` | Test mode lists folders | Yes | Goes away when the Noodle test runner replaces test mode (G4). Until then the bootstrap's test mode doesn't build on plain Windows (MSVC). | in use |

**Not dependencies** (checked): no Python, Node.js, JVM, .NET, Ruby, Perl, make, CMake, shell scripts, or package managers are needed to build or test Noodle. Git is used only for version control (§64).

## Trust notes

- The bootstrap performs file reads for the program it runs and for test files. It has no capability model, because grants (§41) are enforced by the Noodle effect gateway, not the bootstrap. The bootstrap is a developer tool for running Noodle's own source and tests, and it must never be offered to users as a way to run untrusted programs.
