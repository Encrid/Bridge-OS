# src/windows/folder_watch.c

On Windows, this file starts a watch of `include/os.h`, and stops it.
Each function keeps the contract of `docs/extern/os.h.md`, *The watch*.

## The reads

A watch keeps two handles, which `open_listing` opens with `FILE_FLAG_OVERLAPPED`. Thus a read of the changes runs while the caller works.
The handles share the folder for read, write and removal, so other programs can remove or move it.

- The read of `folder` asks `ReadDirectoryChangesW` for the names, the sizes and the times of write. It also asks for all the levels when the depth is above 1.
- Windows tells no move of the watched folder itself. Thus the read of `above` asks for the names of the folders in the folder above.
- When that read ends, `src/windows/watch_take.c` compares the path of the folder with `final`. A different path means that the folder moved.
- The root of a drive cannot move. Its final path ends with a backslash, and the watch opens no folder above it.

`os_watch_start` puts a backslash after the whole path, because `CreateFileW` opens the root of a drive only with it.
`GetFinalPathNameByHandleW` gives the final path in two calls: the first gives its size, and the second fills `final`.
A path that changed between the two calls gives a different count, and the start fails.

## The stop

A read that runs writes into the watch when it ends. Thus `os_watch_stop` ends each read with `os_end_read` before it frees the watch.
A read that failed to start does not run. Thus `reading` and `looking` tell which reads to end.

## Functions

| Function | Result |
| --- | --- |
| `open_listing(path)` | Opens the folder `path` for its changes, as *The reads* tells. Gives the handle, or `INVALID_HANDLE_VALUE`. |
| `keep_final(w)` | Keeps the final path of the folder in `final`, in one block with `again`. Tells if it kept it. |
| `watch_above(w)` | Opens the folder above the folder, and starts its read, as *The reads* tells. Tells if it did it, or if the folder is a root. |
| `os_watch_start(folder, depth)` | Makes the watch with `calloc`, opens the folder and the folder above, and starts their reads. Gives `nullptr` for a depth of 0, or if one of these fails. |
| `os_watch_stop(w)` | Ends the reads, as *The stop* tells, closes the handles, and frees the path and the watch. A null `w` does nothing. |
