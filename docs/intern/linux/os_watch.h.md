# src/linux/os_watch.h

This header gives `include/os.h` its watch on Linux, and declares the parts of the watch for the files of `src/linux`.
It includes `<limits.h>`, `<sys/inotify.h>` and `src/posix/os_types.h`.

## Constants

| Constant | Value |
| --- | --- |
| `OS_WATCH_MASK` | The changes that each watch of `inotify` asks for: a made, changed, closed, moved or removed entry, and a removed or moved folder. `IN_ONLYDIR` refuses a path that is not a folder. |
| `OS_EVENTS_ROOM` | 65536: the bytes of the events of one `read`. |
| `OS_FOLDERS_ROOM` | 16: the first count of the folders of a watch. |

## Types

### os_watched_t

A folder that a watch of `inotify` watches.

| Field | Content |
| --- | --- |
| `wd` | The number of the watch of `inotify`. |
| `parts` | The count of the names of the path: 0 for `folder`. |
| `path` | The path of the folder from `folder`: an empty text for `folder`. |

### struct os_watch

The watch of `os_watch_start`, on Linux: `docs/intern/linux/folder_watch.c.md`, *The folders*.

| Field | Content |
| --- | --- |
| `fd` | The descriptor of `inotify`, or -1. |
| `depth` | The depth of the watch. |
| `stopped` | True after the watch stopped. |
| `folders` | The watched folders, in the order of their `wd`. |
| `count` | The count of the watched folders. |
| `room` | The room of `folders`. |
| `at` | The byte of the next event in `events`. |
| `got` | The bytes of the events in `events`. |
| `base` | The length of `folder` and its `/` in `full`. |
| `full` | `folder`, a `/`, and the path of a folder from `folder`: `docs/intern/linux/watch_tree.c.md`, *The path*. |
| `name` | The last name that `os_watch_take` gave. |
| `events` | The events of the last `read`. |

## Functions

| Function | Result |
| --- | --- |
| `os_add_tree(w, end, parts)` | Watches the folder of `full`, and its folders down to the depth: `docs/intern/linux/watch_tree.c.md`. |
| `os_find_folder(w, wd)` | Gives the watched folder of `wd`, or `nullptr`: `docs/intern/linux/watch_table.c.md`. |
| `os_drop_tree(w, path)` | Stops the watches of the folder `path` and of its folders. |
| `os_drop_folder(w, f)` | Takes the folder `f` out of `folders`. |
| `os_use_event(w, name)` | Uses the next event of `events`: `docs/intern/linux/watch_event.c.md`. |
