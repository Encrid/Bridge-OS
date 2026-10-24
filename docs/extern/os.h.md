# include/os.h

This header gives the operating system as Bridge, its backends and its host need it.
That is address space, shared libraries, threads, the clock, a mutex, an event, and the host's folders, paths and locks.

## The layer

This layer is the only code that differs by platform, and it depends on nothing of ours.
It includes `<stddef.h>` and `<stdint.h>` and no other header.
Bridge reaches memory mapping, shared libraries, threads and the clock through this layer and nowhere else.

The build makes the layer into an archive `os`. A release ships `os.a` and `include/os.h` for each target.
Each project that uses the layer fetches them from the release and links `os.a`: the library, each backend and the host.
The linker takes only the functions that the binary calls, so none imports a function it does not use.

`include/os.h` declares each function once.
`src/posix` holds the functions for Linux and macOS, and `src/windows` holds the same functions for Windows.
The build compiles `src/{system}/*.c`, where `system` is the key of the target in `.toolchain/toolchain.toml`.
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
| `os_thread_start` | Starts a thread that runs `fn(arg)`. Gives `nullptr` on failure. |
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
| `os_event_wait` | Waits until the event is set or `nanos` nanoseconds pass, and consumes a set. With `OS_FOREVER` it has no time limit. The core waits with `OS_FOREVER`; a backend watchdog waits with a deadline. |
| `os_folder_set` | Moves the process into `folder`, so the host runs from the folder of the deployment. Gives 0 on success. |
| `os_folder_make` | Makes `folder` when it is not there. Gives 0 on success. |
| `os_path_whole` | Gives the absolute path of `path`, which the caller frees, or `nullptr`. |
| `os_lock` | Takes an exclusive lock on the file `path`, so one host runs a deployment at a time. Gives 0 on success. |
