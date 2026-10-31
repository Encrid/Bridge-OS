# src/macos/core_calls.c

On macOS, this file loads CoreServices for a watch of `include/os.h`, and makes the calls of it that can fail.

## The load

`os_core_load` opens `/System/Library/Frameworks/CoreServices.framework/CoreServices` with `os_lib_open`.
Then `os_lib_get_symbol` gives each part in the order of `enum os_core`. The handle of CoreServices also gives the parts of CoreFoundation, which CoreServices loads.
Thus a program that links `os.a` names no framework, and a build for macOS needs no SDK.

Each call that can fail is a function of its own. Thus a case can make it fail by its name, as `docs/contributing/tests.md`, *Failed calls*, tells.

## Functions

| Function | Result |
| --- | --- |
| `os_core_load(w)` | Loads CoreServices and its parts, as *The load* tells. Tells if each part was found. |
| `os_core_string(w)` | Gives the path of the folder as a `CFStringRef`, or `nullptr`. |
| `os_core_array(w, real)` | Gives a `CFArrayRef` that holds `real`, or `nullptr`. |
| `os_core_create(w, paths, on_events)` | Gives a stream of FSEvents for `paths`, with `on_events` as its callback, or `nullptr`. |
| `os_core_start(w)` | Starts the stream. Sets `started`, and tells if the stream started. |
