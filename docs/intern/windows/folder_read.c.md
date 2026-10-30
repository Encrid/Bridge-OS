# src/windows/folder_read.c

On Windows, this file lists the entries of a folder of `include/os.h`, and reads a file of the folder.
Each function keeps the contract of `docs/extern/os.h.md`, *Functions*.

## The entries

`os_folder_read_entry` gives the waiting first entry, then each entry of `FindNextFileW`. It skips `.` and `..`.
It writes the name in UTF-8 into `name` of the folder, which holds 768 bytes. A name of 255 UTF-16 units needs at most 765.
`FILE_ATTRIBUTE_DIRECTORY` tells if the entry is a folder.

## The read of a file

`os_folder_read_file` writes a backslash and the name after the whole path, in the room that the path keeps.
A name of more than 255 characters does not fit, and the read fails. No file of Windows has such a name.
`_wfopen` opens the file by that path, and `_fstat64` gives its size. `_wfopen` refuses a folder.
`os_read_stream` reads the bytes: `docs/intern/common/read_stream.c.md`, *The buffer*.

## Functions

| Function | Result |
| --- | --- |
| `os_folder_read_entry(f, is_folder)` | Gives the next name of the search, and its kind, as *The entries* tells. Gives `nullptr` at the end. |
| `os_folder_read_file(f, name, len)` | Gives the bytes of the file `name` of *The read of a file*, or `nullptr` if `_wfopen` or the read fails. |
