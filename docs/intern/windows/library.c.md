# src/windows/library.c

This file gives `include/os.h` its shared libraries on Windows: a library opened, looked into, pinned and closed.
Each function keeps the contract of `docs/extern/os.h.md`, *Functions*.

## The path

`widened` turns a path of UTF-8 into the wide form of Windows, with `MultiByteToWideChar`.
A name with no `/` and no `\` is a bare name, and `widened` keeps it as it is.
For a path, it makes the path full with `GetFullPathNameW`, since Windows looks beside a library only for a full path.

## Opening

`os_lib_open` gives a bare name to `LoadLibraryW` and the search of Windows.
For a path, it opens the full path with `LoadLibraryExW` and `LOAD_WITH_ALTERED_SEARCH_PATH`.
That flag asks Windows to look beside the library for what the library needs.

## Pinning

`os_lib_pin` makes the same path, and calls `GetModuleHandleExW` with `GET_MODULE_HANDLE_EX_FLAG_PIN`.
Windows then keeps the library until the end of the process, no matter how many times `FreeLibrary` closes it.
If no library of that path is open, `GetModuleHandleExW` fails, and `os_lib_pin` gives -1.

## Limits

A path must be shorter than `MAX_PATH` wide characters, and so must its full form.
For a longer path, `MultiByteToWideChar` or `GetFullPathNameW` fails, and `widened` gives false.

## Functions

| Function | Result |
| --- | --- |
| `widened(path, full, bare)` | Gives the wide form of a path into `full`, as *The path* tells, and tells if the name is bare. Gives false for a path that is too long. |
| `os_lib_open` | Opens a bare name with `LoadLibraryW`, and a path with `LoadLibraryExW`. Gives `nullptr` on failure. |
| `os_lib_get_symbol` | Gives the result of `GetProcAddress` for `name` in the library: the address, or `nullptr`. |
| `os_lib_close` | Closes the library with `FreeLibrary`. |
| `os_lib_pin` | Keeps the open library of `path` until the end of the process with `GetModuleHandleExW`. Gives 0, or -1. |
