# src/macos/os_queue.h

This header declares the queue of a watch on macOS, for `src/macos/os_watch.h` and `src/macos/watch_queue.c`.
It includes no header. The queue is a `void *`, so a file that includes `include/os.h` needs no header of libdispatch.

## Functions

| Function | Result |
| --- | --- |
| `os_queue_make()` | Makes the queue of a watch: `docs/intern/macos/watch_queue.c.md`. |
| `os_queue_wait(queue)` | Waits until the queue ran each work that it holds. |
| `os_queue_free(queue)` | Frees the queue. |
