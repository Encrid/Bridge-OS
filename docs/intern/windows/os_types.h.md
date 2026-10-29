# src/windows/os_types.h

This header gives `include/os.h` its thread, its mutex, its event and the state of its signals on Windows.
It defines `WIN32_LEAN_AND_MEAN`, and includes `<stdatomic.h>`, `<windows.h>` and `include/os.h`. Each other file of `src/windows` includes this header.

## Types

### struct os_thread

A thread that `os_thread_start` makes.

| Field | Content |
| --- | --- |
| `handle` | The `HANDLE` of the thread. |
| `fn` | The function that the thread runs. |
| `arg` | The argument of `fn`. |

### struct os_mutex

A mutex: a critical section.

| Field | Content |
| --- | --- |
| `section` | The `CRITICAL_SECTION`. |

### struct os_event

An event: an event object of Windows, which resets itself as a wait returns for it.

| Field | Content |
| --- | --- |
| `handle` | The `HANDLE` of the event object. |

### struct os_stop

The state of the signals: `docs/intern/windows/signal.c.md`, *The state*.

| Field | Content |
| --- | --- |
| `caught` | True after the first event. |
| `line` | The line that a thread writes at the first event. |
| `length` | The number of bytes of `line`. |

## Functions

The header declares no function.
