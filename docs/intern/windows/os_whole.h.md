# src/windows/os_whole.h

This header declares the whole path for `src/windows/folder_open.c`, `src/windows/folder_watch.c` and `tests/whole_test.c`.
It includes only `<stddef.h>`, so a program of each system can include it.

## Constants

`OS_PLACE` is 7: the length of `\\?\UNC`, the longest prefix of the form. The whole path starts that far into its buffer.

## Functions

| Function | Result |
| --- | --- |
| `os_mark_whole(whole, size)` | Gives the whole path its form: `docs/intern/windows/whole_form.c.md`, *The form*. |
| `os_get_whole(path)` | Gives the whole path of `path`: `docs/intern/windows/folder_open.c.md`, *The path*. |
