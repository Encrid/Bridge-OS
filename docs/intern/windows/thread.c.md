# src/windows/thread.c

This file gives `include/os.h` its threads and its clock on Windows.
Each function keeps the contract of `docs/extern/os.h.md`, *Functions*.

## The clock

`os_get_now` reads the frequency and the count of the performance counter at each call.
It divides the count by the frequency into whole seconds and a rest, and scales each part to nanoseconds apart.
So the product stays inside 64 bits: `whole * OS_SECOND + rest * OS_SECOND / ticks`, where `ticks` is the frequency.

## The stack

`os_thread_start` gives `stack` to `CreateThread` with the flag `STACK_SIZE_PARAM_IS_A_RESERVATION`.
Thus `stack` is the address space that Windows reserves for the stack, and not the part that it commits first.
Windows rounds the reservation up. It refuses a reservation that it cannot make, for example one of `SIZE_MAX` bytes.
For a `stack` of 0, Windows reserves the size that the program states. That also gives at least `stack` bytes.

## Limits

`os_thread_start` frees the record and gives `nullptr` if `CreateThread` fails.

## Functions

| Function | Result |
| --- | --- |
| `run_thread` | The start function that `CreateThread` gets. It runs `fn(arg)` of the record, then gives 0. |
| `os_thread_start` | Keeps `fn` and `arg` in a record from `malloc`, and starts the thread at once with the stack of *The stack*. Gives `nullptr` when `malloc` or `CreateThread` fails. |
| `os_thread_join` | Waits for the thread with `WaitForSingleObject` and `INFINITE`, closes its handle, and frees its record. |
| `os_get_thread_id` | Gives the result of `GetCurrentThreadId`, which Windows never makes 0. |
| `os_get_now` | Gives nanoseconds from the performance counter, as *The clock* tells. |
