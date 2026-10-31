# src/macos/folder_watch.c

On macOS, this file makes a watch of `include/os.h` from its parts, and frees it.
Each function keeps the contract of `docs/extern/os.h.md`, *The watch*.

## The start

`make_parts` makes the parts of the watch in this order, and stops at the first that fails:

1. `os_path_whole` gives the real path of the folder, because FSEvents gives the real paths of the changes.
2. `os_folder_open` opens the folder, so a file or a missing folder fails.
3. `os_mutex_create` makes the mutex, and `os_queue_make` makes a serial queue for the callback.
4. `os_core_load` loads CoreServices, and `os_open_stream` starts the stream.

## The stop

`os_core_end` ends the stream and its callbacks: `docs/intern/macos/core_ends.c.md`.
Then `os_watch_stop` frees the queue, CoreServices and the watch.

## Functions

| Function | Result |
| --- | --- |
| `is_folder(path)` | Tells if the folder `path` opens. |
| `make_parts(w, folder)` | Makes the parts of *The start*. Tells if each part was made. |
| `os_watch_start(folder, depth)` | Makes the watch with `calloc`, and its parts. A depth of 0 or a failed part gives `nullptr`. |
| `os_watch_stop(w)` | Ends the stream, as *The stop* tells, and frees the parts and the watch. A null `w` does nothing. |
