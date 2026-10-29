# src/windows/signal.c

This file gives `include/os.h` its signals on Windows, which are the console events.
Each function keeps the contract of `docs/extern/os.h.md`, *Functions*.

## The flag

The flag and a second signal work as on Linux and macOS: `docs/intern/posix/signal.c.md`, *The handler*.
Windows calls the handler on a thread of its own for each event, so the flag is atomic here too.

## The handler

`os_signal_catch` gives `catch_stop` to `SetConsoleCtrlHandler`.
After Ctrl-C and Ctrl-Break, `catch_stop` gives `TRUE`, and the process goes on.
Windows ends the process when the handler returns from an event of `ENDING`: the close of the console, a logoff or a shutdown.
So for these events, `catch_stop` sleeps with no end, and the host ends the process itself.
Windows ends it after some seconds in any case.

`is_ending` looks for the event in the table `ENDING`. Thus one condition tells the 3 events apart from the others.
`catch_stop` does not sleep after Ctrl-C or Ctrl-Break.
Windows calls the handler for one event at a time, so a handler that sleeps keeps a second event out.

A parent that makes a process ignore Ctrl-C sets a flag of the process, which a child gets too.
`os_signal_catch` does not clear that flag, so an ignored Ctrl-C stays ignored.

## Functions

| Function | Result |
| --- | --- |
| `get_caught` | Gives the flag. |
| `is_ending(event)` | Tells if the event is in `ENDING`. |
| `catch_stop(event)` | The handler of *The handler*. |
| `os_signal_catch` | Gives `catch_stop` to `SetConsoleCtrlHandler`. Gives 0 on success. |
| `os_signal_is_caught` | Gives the flag. |
