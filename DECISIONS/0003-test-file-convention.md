# 0003 — Test file convention

**Status:** ACCEPTED · 2026-09-24

A test is `name.noodle` + `name.expected`, with an optional `name.input`. The expected file holds everything the program printed, errors included, in order, byte for byte. Answers from `.input` are echoed after each question, so the expected file reads as a transcript. A missing `.expected` file is a failure.

**Why:** it doesn't depend on any language, so the bootstrap's test mode, the Noodle test runner (G4), the World VM, and native code can all run the same files. That is how Law 9 will be checked.
