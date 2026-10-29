# src/macos/stack.c

This file gives `os_thread_stack_left` on macOS, from the fields that libpthread keeps for each thread.
Each function keeps the contract of `docs/extern/os.h.md`, *The stack of a thread*.

## The end of the stack

`pthread_get_stackaddr_np` gives the highest address of the stack of the thread, and `pthread_get_stacksize_np` gives its size.
The lowest address is the highest address less the size. The guard page of the thread is below it.
For the calling thread, both functions read fields of the thread, with no lock and no system call.
For the first thread, libpthread reads `RLIMIT_STACK` one time, and keeps it.

## Functions

| Function | Result |
| --- | --- |
| `os_thread_stack_left()` | Gives the count of bytes between its frame and the lowest address of *The end of the stack*. |
