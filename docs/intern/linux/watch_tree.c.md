# src/linux/watch_tree.c

On Linux, this file watches a folder and its folders down to the depth of a watch of `include/os.h`.
Each function keeps the contract of `docs/extern/os.h.md`, *The watch*.

## The path

`full` holds the path of the folder to watch. A folder in it adds a `/` and its name, and the watch of the folder takes away that part again.
`full` has room for `PATH_MAX` bytes and an end, so a cut path has `PATH_MAX` bytes. `inotify_add_watch` refuses such a path with `ENAMETOOLONG`.
The watch starts before the walk of the folder, so a cut path is never opened.

## The table

`keep_folder` adds a folder at the end of `folders`. `inotify` gives each new watch a number above the numbers before it, so `folders` stays in order.
A folder that has a watch already gets the same number. Then `keep_folder` keeps the new path in its place, because the folder can have moved.
`grow_table` doubles the room of `folders` when it is full.

## Functions

| Function | Result |
| --- | --- |
| `grow_table(w)` | Makes room for one more folder. Tells if the room is there. |
| `keep_folder(w, wd, parts)` | Keeps the folder of `full` with the number `wd`, as *The table* tells. Tells if it kept it. |
| `add_folders(w, in, end, parts)` | Watches each folder of the open folder `in`, with `os_add_tree`. Gives 0, or -1 at the first failure. |
| `os_add_tree(w, end, parts)` | Watches the folder of `full`, whose path ends at `end`, and its folders while their entries are within the depth. Gives 0, or -1 if a watch or an open fails. |
