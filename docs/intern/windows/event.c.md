# src/windows/event.c

This file gives `include/os.h` its event on Windows: an event object that resets itself when a wait returns for it.
Each function keeps the contract of `docs/extern/os.h.md`, *Functions*.

## The wait

`os_event_wait` rounds the rest of `nanos` up to whole milliseconds, and gives them to `WaitForSingleObject`.
Thus a wait of less than a millisecond does not return at once.
A rest of `INFINITE` milliseconds or more, about 49.7 days, gets `INFINITE`, and thus has no time limit. `OS_FOREVER` is such a wait.
The event object resets itself when the wait returns for it, so the wait consumes the event.

Windows can end a time-out up to one tick of its clock before its time.
The page of `Sleep` on Microsoft Learn tells it: https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-sleep.
So after a time-out, `os_event_wait` measures the time that passed with `os_get_now`. While `nanos` have not passed, it calls `WaitForSingleObject` again.
The loop runs while the time that passed is `nanos` or less. Thus a wait of 0 nanoseconds also waits one time, and consumes a set.

## Limits

`os_event_create` checks the result of `CreateEventA`, and gives `nullptr` if it fails.

## Functions

| Function | Result |
| --- | --- |
| `os_event_create` | Allocates the event with `malloc`, and makes an event object with `CreateEventA`. The object resets itself, starts not set, and has no name. Gives `nullptr` if there is no memory, or no event object. |
| `os_event_free` | Closes the handle of the event object, and frees the event. A null event does nothing. |
| `os_event_set` | Sets the event object with `SetEvent`. |
| `os_event_wait` | Waits with `WaitForSingleObject` until the event is set or `nanos` pass on `os_get_now`, as *The wait* tells. |
