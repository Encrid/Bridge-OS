# src/posix/event.c

This file gives the event of `include/os.h` on Linux and macOS: a flag, its mutex, and the condition that a wait sleeps on.
Each function keeps the contract of `docs/extern/os.h.md`, *Functions*.

## The wait

`os_event_wait` takes the mutex. With `OS_FOREVER` it waits on the condition while the flag is not set,
and a wake with the flag not set waits again. Else `os_cond_wait` waits until the flag is set or `nanos`
nanoseconds pass. At the end, the function clears the flag: the wait consumes the event.

Linux and macOS measure a timed wait on a condition in different ways.
So `src/linux` and `src/macos` each give `os_cond_start`, which starts the condition, and `os_cond_wait`.

## Limits

`os_event_create` checks the results of `pthread_mutex_init` and `os_cond_start`, and gives `nullptr` if one fails.

## Functions

| Function | Result |
| --- | --- |
| `os_event_create` | Allocates the event with `malloc`, starts its mutex with the default attributes and its condition with `os_cond_start`, and clears its flag. Gives `nullptr` if there is no memory. |
| `os_event_free` | Destroys the condition and the mutex of the event, and frees it. A null event does nothing. |
| `os_event_set` | Sets the flag under the mutex, and wakes a waiting thread with `pthread_cond_signal`. |
| `os_event_wait` | With `OS_FOREVER`, waits with `pthread_cond_wait` until the flag is set. Else waits with `os_cond_wait`. Clears the flag. |
