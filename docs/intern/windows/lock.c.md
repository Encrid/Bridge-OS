# src/windows/lock.c

`os_lock` takes the run lock for a Windows process.

The call creates or opens the path, then locks one byte with `_locking` under `_LK_NBLCK`.
A value of 0 means the lock is held, and -1 means the open did not work.
A value of 1 means that an earlier lock owns the byte, in a different process or in this one.
The handle closes after `_locking` reports a conflict.

## Functions

| Function | Result |
| --- | --- |
| `os_lock` | Locks one byte with `_locking`. Answers 0, 1 or -1. |
