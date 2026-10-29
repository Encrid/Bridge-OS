# src/windows/event.c

This file gives `include/os.h` its event on Windows: an event object that resets itself when a wait returns for it.
Each function keeps the contract of `docs/extern/os.h.md`, *Functions*.

## The wait

`os_event_wait` rounds `nanos` up to whole milliseconds, and gives them to `WaitForSingleObject`.
Thus a wait of less than a millisecond waits for one millisecond or more, and does not return at once.
A wait of `INFINITE` milliseconds or more, about 49.7 days, gets `INFINITE`, and thus has no time limit. `OS_FOREVER` is such a wait.
Windows counts the time of the wait from its start, so the time of day has no part in it.
The event object resets itself when the wait returns for it, so the wait consumes the event.

## Limits

`os_event_create` checks the result of `CreateEventA`, and gives `nullptr` if it fails.

## Functions

| Function | Result |
| --- | --- |
| `os_event_create` | Allocates the event with `malloc`, and makes an event object with `CreateEventA`. The object resets itself, starts not set, and has no name. Gives `nullptr` if there is no memory, or no event object. |
| `os_event_free` | Closes the handle of the event object, and frees the event. A null event does nothing. |
| `os_event_set` | Sets the event object with `SetEvent`. |
| `os_event_wait` | Waits with `WaitForSingleObject` for `nanos` rounded up to milliseconds, as *The wait* tells. |
