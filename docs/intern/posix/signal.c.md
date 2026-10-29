# src/posix/signal.c

This file gives the signals of `include/os.h` on Linux and macOS: a stop that the host reads, and an end at once.
Each function keeps the contract of `docs/extern/os.h.md`, *Functions*.

## The flag

The flag is a `static` `atomic_bool` in `get_caught`, because a handler of a signal can reach only an object of static storage.
An atomic object that has no lock is safe in a handler, and `atomic_bool` has no lock on each target.

## The handler

`os_signal_catch` gives `catch_stop` to `sigaction` for `SIGINT`, `SIGTERM` and `SIGHUP`, through `catch_one`.
`catch_stop` sets the flag with `atomic_exchange`, which also gives the value before the set.
If the flag was set already, the signal is a second one, and `catch_stop` calls `_Exit` with 1.
`_Exit` ends the process with no handler of `atexit` and no flush of a stream, so the end is at once.
The two calls are safe in a handler of a signal.

`catch_one` first reads the action of the signal. If it is `SIG_IGN`, the parent wants the signal ignored, and `catch_one` keeps it.
`SA_RESTART` makes a call that a signal interrupts start again. Thus a stop does not make a read or a write of a module fail.
An empty `sa_mask` blocks no other signal while the handler runs, because `atomic_exchange` already orders two signals.

## The broken pipe

`os_signal_catch` sets `SIGPIPE` to `SIG_IGN`. A write to a pipe that nobody reads then fails with `EPIPE`, and the process goes on.

## Functions

| Function | Result |
| --- | --- |
| `get_caught` | Gives the flag of *The flag*. |
| `catch_stop(number)` | The handler of *The handler*. It sets the flag, or ends the process with status 1 if the flag was set. |
| `catch_one(number, act)` | Gives `act` to the signal `number`, if the signal is not ignored. Gives what `sigaction` gives, or 0 for an ignored signal. |
| `os_signal_catch` | Calls `catch_one` for each of the 3 signals, and ignores `SIGPIPE`. Gives 0 if each call succeeds. |
| `os_signal_is_caught` | Gives the flag. |
