# src/common/watch_depth.c

This file counts the names in the path of a change, in the same way on each system.
Each function keeps the contract of `docs/extern/os.h.md`, *The watch*.

## The depth

A name of a watch has a `/` between two names, so its count of names is its count of `/` and 1.
`os_is_within` counts the names, and compares their count with `depth`.
The watches of Windows and of macOS use it, because their systems tell the changes of all the levels.

## Functions

| Function | Result |
| --- | --- |
| `os_is_within(name, depth)` | Tells if `name` has at most `depth` names. |
