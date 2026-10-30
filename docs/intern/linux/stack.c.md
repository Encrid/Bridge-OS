# src/linux/stack.c

This file gives `os_thread_stack_left` on Linux, where glibc tells the stack of a thread.
Each function keeps the contract of `docs/extern/os.h.md`, *The stack of a thread*.

## The end of the stack

`system_defines` of `.toolchain/config/targets.toml` defines `_GNU_SOURCE` for Linux, because glibc declares `pthread_getattr_np` only with it.
`read_low` asks `pthread_getattr_np` for the stack of the thread, and `pthread_attr_getstack` gives its lowest address.
For a thread of `pthread_create`, glibc gives the stack without its guard page.
For the first thread, glibc finds the stack in `/proc/self/maps`, and counts `RLIMIT_STACK` down from its end.

## The cost

`pthread_getattr_np` takes the lock of the thread, allocates a set of processors, and asks the kernel for the affinity of the thread.
For the first thread, it also opens and reads `/proc/self/maps`.
Thus `os_thread_stack_left` keeps the lowest address in a `thread_local` variable after its first read. Each later call of the thread reads only that variable.
A failed read keeps 0, so the next call reads again.

## Functions

| Function | Result |
| --- | --- |
| `read_low()` | Gives the lowest address of the stack of the calling thread, or 0 if `pthread_getattr_np` fails. |
| `os_thread_stack_left()` | Reads the lowest address one time for each thread, as *The cost* tells. Gives the distance to it from the frame of the call, or 0. |
