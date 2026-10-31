# src/linux/watch_event.c

On Linux, this file uses one event of `inotify` for a watch of `include/os.h`.
Each function keeps the contract of `docs/extern/os.h.md`, *The watch*.

## The events

| Event | What the watch does |
| --- | --- |
| `IN_Q_OVERFLOW` | The queue of `inotify` lost events. The watch watches the folders again, because a folder can have come in with no event. The take gives 2. |
| An event of a folder that `folders` does not hold | The watch dropped that folder already. The take goes on. |
| `IN_DELETE_SELF`, `IN_MOVE_SELF`, `IN_UNMOUNT` or `IN_IGNORED` of `folder` | `folder` was removed or moved, or its file system went away. The watch stops. |
| `IN_IGNORED` of a folder in `folder` | The system stopped the watch of that folder. `os_drop_folder` takes it out of `folders`. |
| Another event | The take gives the name of the entry: the path of its folder, a `/`, and the name of the event. |

An event that comes for a folder can change the watched folders:

- A folder that is made or moved in gets its watches before the take gives its name, if its path is within `depth` - 1.
- A folder that moves out loses the watches of its folders with `os_drop_tree`. Else its watches tell the changes of a folder that is no longer in `folder`.
- A folder that is removed gives `IN_IGNORED` for each of its watches.

If a watch of a new folder fails, or a watch of the folders again fails, the watch stops.

## Functions

| Function | Result |
| --- | --- |
| `end_folder(w, f, mask)` | Stops the watch for an event of `folder`, or drops the folder `f` for `IN_IGNORED`, as *The events* tells. Gives `OS_AGAIN`. |
| `follow_folder(w, mask)` | Watches or drops the folder of `name`, as *The events* tells. |
| `give_name(w, f, e, name)` | Writes the path of the entry of the event `e` of the folder `f` into `name`, and gives 1. |
| `os_use_event(w, name)` | Uses the next event of `events`, as *The events* tells. Gives 1, 2 or `OS_AGAIN`. |
