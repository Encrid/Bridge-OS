# src/macos/event_clock.c

On macOS, which gives a condition no clock of its own, this file measures a timed wait of `src/posix/event.c`.

## The clock

macOS has no `pthread_condattr_setclock`, and `pthread_cond_timedwait` reads its deadline on the time of day.
So `os_cond_wait` waits with `pthread_cond_timedwait_relative_np`, which takes a span from now.
After a wake, it reads `os_get_now`, and waits again for the rest of `nanos` while the flag is not set.
`os_get_now` reads `CLOCK_MONOTONIC_RAW`, which only rises: `docs/intern/posix/thread.c.md`, *The clock*.

## Functions

| Function | Result |
| --- | --- |
| `os_cond_start(cond)` | Starts `cond` with the default attributes. Gives what `pthread_cond_init` gives: 0 on success. |
| `os_cond_wait(e, nanos)` | Waits with `pthread_cond_timedwait_relative_np`, as *The clock* tells. It ends when `e` is set or `nanos` pass. |
