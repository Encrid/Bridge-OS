# src/macos/os_watch.h

This header gives `include/os.h` its watch on macOS. It also declares what the files of the watch in `src/macos` share.
It includes `src/macos/os_queue.h` and `src/posix/os_types.h`.
No header of the system declares FSEvents for a build with no SDK, so this header declares the parts that the watch uses.

## Constants

| Constant | Value |
| --- | --- |
| `OS_NAMES_ROOM` | 65536: the bytes of the names that a watch keeps between two takes. |
| `OS_EVENTS_LOST` | `kFSEventStreamEventFlagMustScanSubDirs`, which FSEvents sets when it lost events. |
| `OS_EVENTS_ENDED` | `kFSEventStreamEventFlagRootChanged` and `kFSEventStreamEventFlagUnmount`: the folder moved or went away. |
| `OS_STREAM_FLAGS` | `kFSEventStreamCreateFlagNoDefer`, `kFSEventStreamCreateFlagWatchRoot` and `kFSEventStreamCreateFlagFileEvents`. |
| `OS_SINCE_NOW` | `kFSEventStreamEventIdSinceNow`: the stream tells only the changes after its start. |
| `OS_LATENCY` | 0.05: the seconds that FSEvents waits to put later changes together. |

## Types

| Type | Content |
| --- | --- |
| `enum os_core` | The index of each part of CoreServices in `calls`, and `CORE_CALLS`, their count. |
| `os_events_fn_t` | The function that FSEvents calls with a list of changes. Its 6 parameters are the parameters of `FSEventStreamCallback`. |
| `os_string_fn_t`, `os_array_fn_t`, `os_release_fn_t`, `os_create_fn_t`, `os_queue_fn_t`, `os_start_fn_t`, `os_stream_fn_t` | The functions of CoreServices that the watch calls, with their parameters in the types of C. |
| `os_names_t` | A buffer of names, each with its end, and `used`, the count of its bytes. |
| `os_context_t` | The `FSEventStreamContext` of a stream. Its `info` is the watch. |

### struct os_watch

The watch of `os_watch_start`, on macOS: `docs/intern/macos/watch_events.c.md`, *The stream*.

| Field | Content |
| --- | --- |
| `lib` | The handle of CoreServices, or `nullptr`. |
| `calls` | The parts of CoreServices, in the order of `enum os_core`. |
| `stream` | The stream of FSEvents, or `nullptr`. |
| `queue` | The queue of the stream, which `os_queue_make` gave, or `nullptr`. |
| `lock` | The mutex of `filling`, `lost` and `ended`. |
| `real` | The real path of the folder, which `os_path_whole` gave. |
| `skip` | The bytes before the name of a change in a path of FSEvents. |
| `depth` | The depth of the watch. |
| `started` | True after the stream started. |
| `lost` | True after FSEvents lost changes, or after `filling` was full. |
| `ended` | True after the folder moved or went away. |
| `at` | The byte of the next name in `reading`. |
| `filling` | The buffer that the callback fills. |
| `reading` | The buffer that the take reads. |
| `names` | The two buffers. |

## Functions

| Function | Result |
| --- | --- |
| `os_core_load(w)` | Loads CoreServices and its parts: `docs/intern/macos/core_calls.c.md`. |
| `os_core_string(w)`, `os_core_array(w, real)`, `os_core_create(w, paths, on_events)`, `os_core_start(w)` | The calls of CoreServices that can fail. |
| `os_core_queue(w)`, `os_core_release(w, object)`, `os_core_end(w)` | The other calls: `docs/intern/macos/core_ends.c.md`. |
| `os_open_stream(w)` | Makes the stream and starts it: `docs/intern/macos/watch_events.c.md`. |
