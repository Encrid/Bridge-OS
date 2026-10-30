# src/windows/signal.c

This file gives `include/os.h` its signals on Windows, which are the console events.
Each function keeps the contract of `docs/extern/os.h.md`, *Functions*.

## The state

`get_stop` keeps one `struct os_stop` of `src/windows/os_types.h`, which holds the flag, the line and its length.
Windows calls the handler on a thread of its own for each event, so the flag is atomic.
The flag and a second signal work as on Linux and macOS: `docs/intern/posix/signal.c.md`, *The handler*.

## The handler

`os_signal_catch` gives `catch_stop` to `SetConsoleCtrlHandler`.
After Ctrl-C and Ctrl-Break, `catch_stop` gives `TRUE`, and the process goes on.
Windows ends the process when the handler returns from an event of `ENDING`: the close of the console, a logoff or a shutdown.
So for these events, `catch_stop` sleeps with no end, and the host ends the process itself.
Windows ends it after some seconds in any case.

`catch_stop` looks for the event in the table `ENDING`. Thus one condition tells the 3 events apart from the others.
`catch_stop` does not sleep after Ctrl-C or Ctrl-Break.
Windows calls the handler for one event at a time, so a handler that sleeps keeps a second event out.

A parent that makes a process ignore Ctrl-C sets a flag of the process, which a child gets too.
`os_signal_catch` does not clear that flag, so an ignored Ctrl-C stays ignored.

## The line

`catch_stop` does not write the line itself, because a write waits when the output is full.
A handler that waits keeps the second event out, as *The handler* tells. So at the first event, `catch_stop` starts a thread with `CreateThread`, and closes its handle at once.
That thread runs `write_line`, which writes the line with `WriteFile` on the standard output.
If the output is full, only that thread waits, and a second event still ends the process.

## Functions

| Function | Result |
| --- | --- |
| `get_stop` | Gives the state of *The state*. |
| `write_line(stop)` | The function of the thread of *The line*. It writes the line of `stop`, and gives 0. |
| `catch_stop(event)` | The handler of *The handler*. |
| `os_signal_catch(line)` | Stores `line` and its length, and gives `catch_stop` to `SetConsoleCtrlHandler`. Gives 0 on success. |
| `os_signal_is_caught` | Gives the flag. |
