# src/windows/watch_read.c

On Windows, this file starts, checks, finishes and ends the reads of a watch.
Each function keeps the contract of `docs/extern/os.h.md`, *The watch*.

## The results

`os_finish_read` takes the result of a read that ended, with no wait:

| Result of the read | What the watch does |
| --- | --- |
| Changes | `records` holds them, from byte 0 to `got`. |
| 0 bytes | The changes did not fit in the buffer of the system. The take gives 2, and its next step starts a new read. |
| A failure | The folder was removed, or its file system stopped the watch. The watch stops. |

On a removed folder, the read fails with `STATUS_DELETE_PENDING`, which Windows gives as error 5.
A new read of a removed folder also fails with error 5. Then the `OVERLAPPED` stays `STATUS_PENDING`, so the watch keeps `stopped` and asks it no more.

## The path

`os_is_in_place` asks `GetFinalPathNameByHandleW` for the path of the folder again, into `again`, which has the size of `final`.
A longer path does not fit, and the call gives a count that is not the length of `final`. A path of the same length must be equal to `final`.

## Functions

| Function | Result |
| --- | --- |
| `os_read_changes(w)` | Starts the read of the changes of the folder into `records`, as `src/windows/folder_watch.c` tells. Sets `reading`, and tells if the read started. |
| `os_read_above(w)` | Starts the read of the names of the folders above the folder into `above_records`. Sets `looking`, and tells if the read started. |
| `os_is_in_place(w)` | Tells if the folder still has the path of its start, as *The path* tells. |
| `os_finish_read(w)` | Takes the result of the read of the folder, as *The results* tells. Gives 2 for 0 bytes, else `OS_AGAIN`. |
| `os_end_read(handle, overlapped)` | Cancels the read of `overlapped` with `CancelIoEx`, and waits for its end with `GetOverlappedResult`. |
