# src/posix/folder_read.c

This file lists a folder of `include/os.h`, and reads its files, on Linux and macOS.
Each function keeps the contract of `docs/extern/os.h.md`, *Functions*.

## The entries

`os_folder_read_entry` calls `readdir` until it gets a name that is not `.` or `..`.
`fstatat` on the descriptor of the stream tells if the entry is a folder. It follows a link, as `opendir` of the joined path does.
If `fstatat` fails, for example for a link to nothing, the entry counts as no folder, and a read of it fails.

## The read of a file

`os_folder_read_file` opens the name with `openat` on the descriptor of the stream. Thus it never joins the path of the file.
It reads only a regular file: `fstat` must show `S_ISREG`. Then `fdopen` makes a stream of the descriptor.
`os_read_stream` reads the bytes into the buffer of the folder: `docs/intern/common/read_stream.c.md`, *The buffer*.

## Functions

| Function | Result |
| --- | --- |
| `os_folder_read_entry(f, is_folder)` | Gives the next name of `readdir`, and its kind, as *The entries* tells. Gives `nullptr` at the end. |
| `os_folder_read_file(f, name, len)` | Reads the file `name`, as *The read of a file* tells. Gives `nullptr` if it does not open, is not a regular file, or does not read. |
