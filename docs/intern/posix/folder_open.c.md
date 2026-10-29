# src/posix/folder_open.c

This file opens and closes a folder of `include/os.h` on Linux and macOS.
Each function keeps the contract of `docs/extern/os.h.md`, *The folders*.

## The path

`os_folder_open` first gives the path to `opendir`, which opens a path of at most `PATH_MAX` bytes at once.
If `opendir` fails with `ENAMETOOLONG`, `open_path` opens the folder one name at a time, and `fdopendir` makes its stream.
`open_path` starts at `/` for a path that starts with `/`, else at the current folder.
It cuts a copy of the path at each `/` with `strtok_r`, and `step` opens each name from the folder before it.
`strtok_r` gives no empty name, so a path that ends with `/`, or that holds `//`, gives `step` no empty name.

`step` opens a folder before the last with `os_open_search`, for search only, and the last folder with `O_RDONLY`.
Thus the walk needs the same rights as `opendir` of a short path. It needs the right to search each folder before the last, and the right to read the last.

## Limits

If the walk or `fdopendir` fails, `os_folder_open` closes the descriptor. After a failed walk the descriptor is -1, and `close` of -1 does nothing.
A path of only `/` past `PATH_MAX` names no folder, so the walk ends on a descriptor for search only. `fdopendir` refuses it.

## Functions

| Function | Result |
| --- | --- |
| `step(at, name, last)` | Goes one name down the walk of *The path*: it opens `name` in `at`, closes `at`, and gives the new descriptor, or -1. |
| `open_path(path)` | Gives the descriptor of the folder of `path`, opened one name at a time as *The path* tells, or -1. |
| `os_folder_open(path)` | Makes the folder with `calloc`, and opens its stream as *The path* tells. Gives `nullptr` if no stream opens. |
| `os_folder_close(f)` | Closes the stream with `closedir`, and frees the bytes and the folder. A null `f` does nothing. |
