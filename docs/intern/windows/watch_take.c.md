# src/windows/watch_take.c

On Windows, this file gives the changes of a watch, one name at a time, with no wait.
Each function keeps the contract of `docs/extern/os.h.md`, *The watch*.

## The steps

`os_watch_take` does the steps of `docs/intern/common/os_common.h.md`, *Constants*, and then looks at the read of the folder above. A step does the first of these:

1. If `records` holds a change that the take did not give, `hand_out` gives it.
2. If no read of the folder runs, `os_read_changes` starts one. If it does not start, the watch stops.
3. If the read runs and did not end, the step gives 0.
4. Else `os_finish_read` takes its result.

A read writes into `records`, so the next read starts only after `hand_out` gave the last change of the buffer.
Between two reads, the system keeps the changes in its own buffer.
`HasOverlappedIoCompleted` reads the `OVERLAPPED` in memory, with no call of Windows.

## The folder above

When the read of `above` ended, `see_above` asks `os_is_in_place` if the folder has the path of its start.
If it has, `see_above` starts the next read of `above`. If it has not, or if the next read does not start, the watch stops.
A removed folder ends the read of the folder with a failure first, so the steps stop the watch, and `see_above` does nothing.

## The names

`hand_out` turns the name of a `FILE_NOTIFY_INFORMATION` record into UTF-8, and puts a `/` in place of each backslash.
`name` has room for the longest name, so no name needs memory.
A name deeper than `depth` gives `OS_AGAIN`, so the take goes on to the next change.

## Functions

| Function | Result |
| --- | --- |
| `keep_above(w)` | Tells if the folder has the path of its start, and the next read of `above` started. |
| `see_above(w)` | Stops the watch if the folder moved, as *The folder above* tells. |
| `hand_out(w, name)` | Gives the next change of `records` in `*name`, as *The names* tells. Gives 1, or `OS_AGAIN` for a deeper name. |
| `step(w, name)` | Does one step of *The steps*. |
| `os_watch_take(w, name)` | Does *The steps*, and gives their result, or -1 when the watch stopped. |
