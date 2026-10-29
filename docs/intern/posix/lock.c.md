# src/posix/lock.c

`os_lock` takes the run lock on Linux and macOS.

It opens the path with `open`, then asks `flock` for `LOCK_EX` without blocking.
The result is 0 when the process holds the lock, and -1 when the open fails.
It is 1 when an earlier lock holds the file: of another process, or of an earlier call in this process.
It closes the descriptor if `flock` refuses.

## Functions

| Function | Result |
| --- | --- |
| `os_lock` | Takes a non-blocking exclusive lock through `flock`. Gives 0, 1 or -1. |
