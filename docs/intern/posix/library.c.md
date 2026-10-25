# src/posix/library.c

This file gives the shared libraries of `include/os.h` on Linux and macOS: a library opened, looked into, pinned and closed.
Each function keeps the contract of `docs/extern/os.h.md`, *Functions*.

## Opening

`os_lib_open` calls `dlopen` with `RTLD_NOW` and `RTLD_LOCAL`.
`RTLD_NOW` resolves every symbol of the library before `dlopen` returns.
`RTLD_LOCAL` keeps the names of the library to itself: they do not resolve the symbols of a library that opens later.
The loader looks beside a library that a path names, and searches for a bare name, with no flag of this file.

## Pinning

`os_lib_pin` calls `dlopen` with `RTLD_NOLOAD` and `RTLD_NODELETE`.
`RTLD_NOLOAD` opens nothing: `dlopen` gives the library only if it is already open, else `nullptr`.
On Linux, `RTLD_NODELETE` stays on the library, so no later `dlclose` unloads it.
On macOS, the flag did not keep a library after its last `dlclose`. So the file keeps one handle of each pinned library, and never closes it.

The kept handles are in a table of `OS_PINS` entries, under a mutex, because two threads can pin at the same time.
If the table holds the handle already, `os_lib_pin` closes the new handle. Thus a second pin of a library adds no handle.

## Closing

`os_lib_close` calls `dlclose`.

## Functions

| Function | Result |
| --- | --- |
| `os_lib_open` | Opens the library with `dlopen`. Gives `nullptr` on failure. |
| `os_lib_get_symbol` | Gives what `dlsym` finds of `name` in the library, or `nullptr`. |
| `os_lib_close` | Closes the library with `dlclose`. |
| `held(libs, used, lib)` | Tells if the first `used` kept handles of `libs` hold `lib`. |
| `os_lib_pin` | Pins the open library of `path`, as *Pinning* tells. Gives 0, or -1 if `dlopen` finds no such library, or if the table is full. |

## Limits

The table holds `OS_PINS` libraries, which is 64. A pin of one more library gives -1, and closes its new handle.
