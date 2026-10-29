# src/posix/os_types.h

This header gives the thread, the mutex and the event of `include/os.h` where there are POSIX threads: on Linux and macOS.
It includes `<pthread.h>` and `include/os.h`, and the other files of `src/posix` include it.

## Types

### struct os_thread

A thread that `os_thread_start` makes.

| Field | Content |
| --- | --- |
| `handle` | The `pthread_t` of the thread. |
| `fn` | The function that the thread runs. |
| `arg` | The argument of `fn`. |

### struct os_mutex

A mutex.

| Field | Content |
| --- | --- |
| `mutex` | The `pthread_mutex_t`. |

### struct os_event

An event: a flag, the mutex that guards the flag, and a condition for a wait.

| Field | Content |
| --- | --- |
| `lock` | The `pthread_mutex_t` that guards `set`. |
| `cond` | The `pthread_cond_t` that a wait sleeps on. |
| `set` | 1 while the event is set, and 0 when it is not. |

## Functions

The header declares the two functions of a condition that `src/posix/event.c` calls.
`src/linux/event_clock.c` defines them for Linux, and `src/macos/event_clock.c` for macOS.

| Function | Result |
| --- | --- |
| `os_cond_start(cond)` | Starts the condition of an event. Gives 0 on success. |
| `os_cond_wait(e, nanos)` | Waits on the condition of `e` until its flag is set or `nanos` nanoseconds pass. The caller holds the mutex of `e`. |
