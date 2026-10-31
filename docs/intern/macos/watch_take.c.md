# src/macos/watch_take.c

On macOS, this file hands out the names that the callback of a watch kept.
Each function keeps the contract of `docs/extern/os.h.md`, *The watch*.

## The buffers

The callback fills `filling`, and the take reads `reading`. Thus a name stays valid while the callback adds more names.

1. If `reading` holds a name that the take did not give, the take gives it, with no lock.
2. Else the take holds `lock`, and `swap_names` exchanges the two buffers and empties the new `filling`.
3. After `ended`, `swap_names` gives -1 and exchanges no buffer. After `lost`, it empties `reading` too, and gives 2.

A take with no change locks and unlocks the mutex, and makes no call of the system.

## Functions

| Function | Result |
| --- | --- |
| `give_name(w, name)` | Gives the next name of `reading` in `*name`, and gives 1. |
| `swap_names(w)` | Exchanges the buffers, as *The buffers* tells. Gives 0, 2 or -1. |
| `os_watch_take(w, name)` | Gives a name, or the state of the watch, as *The buffers* tells. |
