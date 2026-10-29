# src/windows/stack.c

On Windows, this file tells how much stack the calling thread has left.
Each function keeps the contract of `docs/extern/os.h.md`, *The stack of a thread*.

## The end of the stack

`GetCurrentThreadStackLimits` gives the lowest address of the stack that Windows gave the thread.
Windows ends the process with a stack overflow before a call reaches that address.
On Windows 11 x86_64, the overflow comes when the guarantee of the thread and 8 KiB are left. The guarantee counts as 8 KiB at least.

`SetThreadStackGuarantee` with 0 gives that guarantee in whole pages, and changes nothing.
Thus the count ends above the lowest address by the guarantee, at least `LEAST_GUARANTEE`, and by `GUARD`. Both constants are 8192.

## The cost

Both calls only read values of the thread. Together they took 3.9 ns on Windows 11 x86_64, which is too short for a system call.

## Functions

| Function | Result |
| --- | --- |
| `os_thread_stack_left()` | Gives the bytes between its frame and the end that *The end of the stack* tells. |
