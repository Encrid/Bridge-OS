# src/macos/watch_queue.c

On macOS, this file makes, waits for and frees the queue on which FSEvents calls a watch of `include/os.h`.

## The names of libdispatch

`<dispatch/dispatch.h>` includes `<os/object.h>`, which declares a function `os_release` of its own.
`include/os.h` declares a different `os_release`, so no file can include the two headers.
Thus this file is the only file that includes `<dispatch/dispatch.h>`, and it does not include `include/os.h`.

## The wait

`os_queue_wait` runs `wait_nothing` on the queue with `dispatch_sync_f`. The queue is serial, so this ends after each work that the queue held.

## Functions

| Function | Result |
| --- | --- |
| `wait_nothing(context)` | Does nothing. |
| `os_queue_make()` | Makes a serial queue with `dispatch_queue_create`. Gives `nullptr` if it fails. |
| `os_queue_wait(queue)` | Waits for the queue, as *The wait* tells. |
| `os_queue_free(queue)` | Frees the queue with `dispatch_release`. |
