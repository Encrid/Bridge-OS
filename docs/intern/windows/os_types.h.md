# src/windows/os_types.h

This header gives `include/os.h` its thread, its mutex, its event, its open folder and the state of its signals on Windows.
It defines `WIN32_LEAN_AND_MEAN`, and includes `<stdatomic.h>`, `<windows.h>` and `include/os.h`. Each other file of `src/windows` includes this header.

## Constants

`OS_NAME_ROOM` is 256: the room for a name of 255 characters and its end.
The fields `whole` and `name` of `struct os_folder` keep that room for a name.

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

### struct os_folder

The open folder of `os_folder_open`, on Windows.

| Field | Content |
| --- | --- |
| `find` | The handle of the search of `FindFirstFileExW`, or `INVALID_HANDLE_VALUE`. |
| `data` | The entry that the search found last. |
| `pending` | True while the first entry of the search is not given yet. |
| `whole` | The whole path of the folder in the form `\\?\`, with room after it for a backslash and a name. A path that has the form `\\.\` keeps it. |
| `bytes` | The memory of the file read last, or `nullptr` before the first read. |
| `name` | The last name that `os_folder_read_entry` gave, in UTF-8. |

### struct os_stop

The state of the signals: `docs/intern/windows/signal.c.md`, *The state*.

| Field | Content |
| --- | --- |
| `caught` | True after the first event. |
| `line` | The line that a thread writes at the first event. |
| `length` | The number of bytes of `line`. |

## Functions

The header declares no function.
