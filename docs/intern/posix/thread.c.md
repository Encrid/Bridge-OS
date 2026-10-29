# src/posix/thread.c

This file gives the threads and the clock of `include/os.h` on Linux and macOS.
Each function keeps the contract of `docs/extern/os.h.md`, *Functions*.

## The stack

`os_thread_start` starts the thread with attributes that give the size of its stack.
The size is `stack`, raised to `PTHREAD_STACK_MIN` of `<limits.h>`, then rounded up to whole pages.
`sysconf` gives the size of a page.
Linux and macOS refuse a stack below `PTHREAD_STACK_MIN`, and macOS also refuses a part of a page.
A size within one page of the top of `size_t` rounds to 0, which `pthread_attr_setstacksize` refuses.

## Limits

`os_thread_start` checks the results of `pthread_attr_setstacksize` and `pthread_create`. If one of them fails, it frees the record and gives `nullptr`.

## The clock

`os_get_now` reads `CLOCK_MONOTONIC_RAW`. On macOS, `CLOCK_MONOTONIC` moves in whole microseconds, so a shorter span reads as 0 or as 1000 ns.
The raw clock moves in the ticks of the machine on macOS, and in nanoseconds on Linux.

## Functions

| Function | Result |
| --- | --- |
| `run_thread` | The start function that `pthread_create` gets. It calls `fn(arg)` of the record, and gives `nullptr`. |
| `os_thread_start` | Allocates the record of the thread with `malloc`, keeps `fn` and `arg` in it, and starts the thread with the stack of *The stack*. Gives `nullptr` when `malloc`, `pthread_attr_setstacksize` or `pthread_create` fails. |
| `os_thread_join` | Waits for the thread with `pthread_join`, and frees its record. |
| `os_get_thread_id` | Gives the `pthread_t` of the calling thread as a number, which is never 0 for a thread that runs. |
| `os_get_now` | Gives the nanoseconds of `CLOCK_MONOTONIC_RAW`: its seconds times `OS_SECOND`, plus its nanoseconds. |
