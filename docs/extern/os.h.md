# include/os.h

This header gives the operating system as Bridge, its backends and its host need it.
That is address space, shared libraries, threads, the clock, a mutex, an event, and the host's folders, paths, locks and signals.

## The layer

This layer is the only code that differs by platform, and it depends on nothing of ours.
It includes `<stddef.h>` and `<stdint.h>` and no other header.
Bridge reaches memory mapping, shared libraries, threads and the clock through this layer and nowhere else.

The build makes the layer into an archive `os`. A release ships `os.a`, `include/os.h`, `leaks.toml` and `fail.toml` for each target.
Each project that uses the layer fetches them from the release and links `os.a`: the library, each backend and the host.
`leaks.toml` names each function that takes a resource, and the function that gives it back.
A project names the file in `[leaks] with`, so that `test --leaks` counts the resources of the layer that its code takes.
The linker takes only the functions that the binary calls, so none imports a function it does not use.

`fail.toml` names each function of the layer that can fail: each function that is `[[nodiscard]]`.
With each name, it gives the value of a failed call: `nullptr` for a pointer, and -1 for a number.
A project names the file in `[fail] with`, so that a case of the project can make a call of the layer fail.

`include/os.h` declares each function once.
`src/posix` holds the functions for Linux and macOS, and `src/windows` holds the same functions for Windows.
`src/linux` and `src/macos` hold the parts of `src/posix` where Linux and macOS differ.
The build compiles `src/{system}/*.c`, where `system` is the key of the target in `.toolchain/toolchain.toml`.
It also compiles the files of `system_sources`, the key of the target in `.toolchain/config/targets.toml`.
So no file of the layer asks which platform it is compiled for.

## Constants

Each constant is a `constexpr uint64_t`.

| Constant | Content |
| --- | --- |
| `OS_SECOND` | `1'000'000'000`: the nanoseconds in a second. |
| `OS_FOREVER` | `UINT64_MAX`: a wait of this many nanoseconds has no time limit. |

## Types

### os_thread_t

A thread that `os_thread_start` makes: `struct os_thread`.
Each platform defines the struct in its `os_types.h`.

### os_mutex_t

A mutex: `struct os_mutex`, which each platform defines.

### os_event_t

An event: `struct os_event`, which each platform defines.

### os_thread_fn_t

The function that a thread runs: `void (*)(void *arg)`.

## Functions

Each function that can fail is `[[nodiscard]]`: the caller must use its result.

| Function | Result |
| --- | --- |
| `os_reserve` | Reserves `reserve` bytes of address space, and makes the first `commit` bytes readable, writable and zero. The rest faults when touched, which a backend that enforces bounds by a fault needs after a block. Gives `nullptr` on failure. |
| `os_commit` | Makes the first `bytes` of a reservation readable and writable. A backend grows a module's memory with it, up to the reservation. Gives 0 on success. |
| `os_release` | Gives back all of a reservation. Each caller passes a base that `os_reserve` gave. |
| `os_lib_open` | Opens a shared library, or gives `nullptr`. For a path, the loader looks there for the library and for what it needs, since a backend brings its runtime beside itself. For a bare name, the loader uses the search of the operating system. |
| `os_lib_get_symbol` | Gives where a library has what it exports as `name`, or `nullptr`. |
| `os_lib_close` | Closes a library. Each caller passes a library that `os_lib_open` gave. |
| `os_lib_pin` | Keeps the open library of `path` in the process until its end. A later `os_lib_close` closes the handle, and the library stays. A second pin of a library changes nothing. A process can pin any number of libraries. Gives 0, or -1 if no library of `path` is open. |
| `os_thread_start` | Starts a thread that runs `fn(arg)` on a stack of at least `stack` bytes. The system can give more, because it rounds the size up to its unit and to its least stack. It also keeps a small part of the stack for itself. Gives `nullptr` on failure, for example for a stack that does not fit in the address space. |
| `os_thread_join` | Waits for a thread to end, and frees what `os_thread_start` gave. |
| `os_get_thread_id` | Gives a number for the calling thread that no other live thread has. It is never 0. |
| `os_get_now` | Gives nanoseconds from a clock that only rises. On each target, its step is below a microsecond. |
| `os_mutex_create` | Makes a mutex. Gives `nullptr` on failure. |
| `os_mutex_free` | Frees a mutex that no thread holds. A null `m` does nothing. |
| `os_mutex_lock` | Takes a mutex, and waits while another thread holds it. |
| `os_mutex_unlock` | Gives a mutex back. |
| `os_event_create` | Makes an event that one thread sets and another waits for. A set before the wait is not lost, and a wait consumes it. Gives `nullptr` on failure. |
| `os_event_free` | Frees an event that no thread waits for. A null `e` does nothing. |
| `os_event_set` | Sets an event: one wait, now or later, returns for it. |
| `os_event_wait` | Waits until the event is set or `nanos` nanoseconds pass, and consumes a set. The clock of `os_get_now` measures the nanoseconds, so a step of the wall clock does not change a wait. On Windows, the wait rounds `nanos` up to whole milliseconds. With `OS_FOREVER` it has no time limit. The core waits with `OS_FOREVER`; a backend watchdog waits with a deadline. |
| `os_folder_set` | Moves the process into `folder`, so the host runs from the folder of the deployment. Gives 0 on success. |
| `os_folder_make` | Makes `folder` when it is not there. Gives 0 on success. |
| `os_path_whole` | Gives the absolute path of `path`, which the caller frees, or `nullptr`. |
| `os_lock` | Takes an exclusive lock on the file `path`, so one host runs a deployment at a time. It does not wait: it gives 0 on success, 1 if a lock holds the file already, and -1 if the file does not open. The mode is fixed, because the host needs only this mode, and the layer has one implementation of it. |
| `os_signal_catch` | Catches the signals that stop the process, and writes `line` at the first signal, as *The signals* tells. Gives 0 on success. |
| `os_signal_is_caught` | Tells if a signal set the flag of `os_signal_catch`. The host reads it between two passes, and then ends its work. |

## The signals

After `os_signal_catch`, an interrupt, a termination or a hang-up sets a flag, and the process goes on.
A second signal ends the process at once, with status 1, also while the output is full.

At the first signal, the layer writes `line` to the standard output at once, also while a pass of the host hangs.
`line` holds its own line end, and stays valid until the end of the process, for example as a literal.
The layer writes it with one call of the system, past the buffer of `stdio`.
So a host that keeps its lines in a buffer must flush each line as it writes it, for example with `fflush`.
Else the line of the signal can come before older lines. On Windows, `_IOLBF` gives a full buffer, so a line buffer does not do it.

On Linux and macOS, these signals are `SIGINT`, `SIGTERM` and `SIGHUP`.
A signal that the parent made the process ignore stays ignored, for example `SIGHUP` under `nohup`.
The process also ignores `SIGPIPE`. Thus a write to a closed output fails, and does not end the process.

On Windows, the signals are the console events: Ctrl-C, Ctrl-Break, the close of the console, a logoff and a shutdown.
A parent can make a process ignore only Ctrl-C, and then Ctrl-C stays ignored.
After a close, a logoff or a shutdown, Windows ends the process some seconds later in any case.
Windows has no `SIGPIPE`: a write to a closed output fails.
