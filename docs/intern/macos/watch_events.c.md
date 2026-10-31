# src/macos/watch_events.c

On macOS, this file makes the stream of FSEvents of a watch of `include/os.h`, and keeps the names of its changes.
Each function keeps the contract of `docs/extern/os.h.md`, *The watch*.

## The stream

The stream watches the real path of the folder, with events for each file, from `OS_SINCE_NOW`.
FSEvents calls `on_events` on the queue of the watch, with the paths and the flags of the changes. The callback holds `lock` while it adds them.
`kFSEventStreamCreateFlagWatchRoot` makes FSEvents tell a move or a removal of the folder, and also of a folder above it.

`add_event` does these steps for each change:

1. A flag of `OS_EVENTS_ENDED` sets `ended`, and a flag of `OS_EVENTS_LOST` sets `lost`.
2. A path of the folder itself, or a path deeper than `depth`, gives no name.
3. Else the name after the first `skip` bytes goes into `filling`. If it does not fit, `lost` is set.

`skip` is the length of the real path and of its `/`. The root `/` has its `/` already.
The rule F5 of `docs/contributing/coding_style.md` has one exception for `on_events`, because FSEvents fixes its 6 parameters.

## Functions

| Function | Result |
| --- | --- |
| `add_event(w, path, flag)` | Keeps one change, as *The stream* tells. |
| `on_events(stream, info, count, paths, flags, ids)` | Keeps each change of a list from FSEvents under `lock`. |
| `os_open_stream(w)` | Makes the stream with the path and `on_events`, puts it on the queue, and starts it. Tells if it started. |
