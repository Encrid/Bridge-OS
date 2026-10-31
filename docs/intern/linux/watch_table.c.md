# src/linux/watch_table.c

On Linux, this file finds and drops the watched folders of a watch of `include/os.h`.
Each function keeps the contract of `docs/extern/os.h.md`, *The watch*.

## The search

`folders` is in the order of the numbers of the watches, so `bsearch` finds a folder by its number.
`os_drop_tree` keeps the order: it moves each folder that stays down over the dropped ones.

## Functions

| Function | Result |
| --- | --- |
| `compare_wd(key, item)` | Compares the number `key` with the number of the folder `item`, for `bsearch`. |
| `os_find_folder(w, wd)` | Gives the watched folder of the number `wd`, or `nullptr`. |
| `is_under(folder, path, len)` | Tells if `folder` is the folder `path` of `len` bytes, or a folder in it. |
| `os_drop_tree(w, path)` | Stops the watch of the folder `path` and of each folder in it with `inotify_rm_watch`, and takes them out of `folders`. |
| `os_drop_folder(w, f)` | Frees the path of `f`, and takes it out of `folders`. |
