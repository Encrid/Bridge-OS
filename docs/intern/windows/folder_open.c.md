# src/windows/folder_open.c

On Windows, this file opens a folder of `include/os.h` by its whole path, and closes it.
Each function keeps the contract of `docs/extern/os.h.md`, *The folders*.

## The path

`get_whole` turns the path of UTF-8 into wide characters with `widen`.
Then `GetFullPathNameW` makes it whole: it puts the current folder before a relative path. It takes a long path also with no opt-in of Windows.
It writes the whole path at `OS_PLACE` in the buffer, and `os_mark_whole` gives it its form.

`docs/intern/windows/whole_form.c.md` tells that form.
The buffer of the path has room for 256 more characters. `src/windows/folder_read.c` writes a backslash and a name of 255 characters there.

## The search

`os_folder_open` puts `\*` after the whole path for `FindFirstFileExW` with `FindExInfoBasic`, and then removes it.
The first entry of the search waits in `data` until the first call of `os_folder_read_entry`.

## Functions

| Function | Result |
| --- | --- |
| `widen(text)` | Gives a copy of `text` in wide characters, made with `MultiByteToWideChar`, or `nullptr` if there is no memory. |
| `get_whole(path)` | Gives the whole path of `path` in the form of *The path*, or `nullptr`. |
| `os_folder_open(path)` | Makes the folder with `calloc`, and starts the search of *The search*. Gives `nullptr` if the path or the search fails. |
| `os_folder_close(f)` | Ends the search with `FindClose`, and frees the path, the bytes and the folder. A null `f` does nothing. |
