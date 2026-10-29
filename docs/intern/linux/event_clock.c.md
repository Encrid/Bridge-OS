# src/linux/event_clock.c

This file gives the timed wait of `src/posix/event.c` on Linux, on the clock `CLOCK_MONOTONIC`.

## The clock

`os_cond_start` gives the condition the clock `CLOCK_MONOTONIC` with `pthread_condattr_setclock`.
That clock only rises, and a change of the time of day does not move it.
`os_cond_wait` reads the clock, and adds `nanos` to it for the deadline of `pthread_cond_timedwait`.
It adds the seconds and the nanoseconds apart, so no sum passes 64 bits.
After a wake, it waits again while the flag is not set and the deadline has not passed.

## Functions

| Function | Result |
| --- | --- |
| `os_cond_start(cond)` | Starts `cond` on `CLOCK_MONOTONIC`. Gives what `pthread_cond_init` gives: 0 on success. |
| `os_cond_wait(e, nanos)` | Waits with `pthread_cond_timedwait` until the flag of `e` is set, or until the deadline. |
