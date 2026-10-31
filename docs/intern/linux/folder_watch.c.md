# src/linux/folder_watch.c

On Linux, this file starts a watch of `include/os.h`, gives its changes, and stops it.
Each function keeps the contract of `docs/extern/os.h.md`, *The watch*.

## The folders

A watch has one descriptor of `inotify`, made with `IN_NONBLOCK`. Thus a `read` with no event gives -1 at once.
It watches `folder`, and each folder in it down to `depth` - 1 levels. A watch of a folder tells the changes of its entries, so no name is deeper than `depth`.
`folders` keeps the path of each watched folder by the number of its watch, because an event gives that number and a name.

`os_watch_start` makes the path of `folder` whole with `os_path_whole`. Thus a new folder gets its watch also after a change of the current folder.

## The steps

`os_watch_take` does the steps of `docs/intern/common/os_common.h.md`, *Constants*. A step does the first of these:

1. If `events` holds an event that the take did not use, `os_use_event` uses it.
2. Else one `read` fills `events`, and the take goes on.
3. Else the take gives 0.

The descriptor does not block, and `events` holds the largest event. Thus `read` fails only with `EAGAIN`, and a failed `read` gives 0.

## Functions

| Function | Result |
| --- | --- |
| `os_watch_start(folder, depth)` | Makes the watch and `folders` with `calloc`, makes the descriptor and the whole path, and watches the folders of *The folders*. A depth of 0, or a failure of one of them, gives `nullptr`. |
| `os_watch_stop(w)` | Closes the descriptor, which stops each watch of it, and frees the paths, `folders` and the watch. A null `w` does nothing. |
| `step(w, name)` | Does one step of *The steps*. |
| `os_watch_take(w, name)` | Gives the next change: 1, 0, 2 or -1, as *The steps* tells. Gives -1 after the watch stopped. |
