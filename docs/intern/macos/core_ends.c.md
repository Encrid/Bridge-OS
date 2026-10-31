# src/macos/core_ends.c

On macOS, this file makes the calls of CoreServices that cannot fail, for a watch of `include/os.h`.

## The end of a stream

FSEvents refuses `FSEventStreamStop` for a stream that did not start, so `os_core_end` stops only a stream that started.
Each stream is on its queue before its start, so `FSEventStreamInvalidate` can take it off.
Then `os_queue_wait` waits until the queue ran each callback that it held.

## Functions

| Function | Result |
| --- | --- |
| `os_core_queue(w)` | Puts the stream on the queue of the watch with `FSEventStreamSetDispatchQueue`. |
| `os_core_release(w, object)` | Releases an object of CoreFoundation with `CFRelease`. |
| `os_core_end(w)` | Stops, takes off and releases the stream, and waits for the queue, as *The end of a stream* tells. |
