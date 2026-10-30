# src/common/read_stream.c

This file reads a whole stream into the buffer of a folder, in the same way on each system.

## The buffer

`os_read_stream` resizes the buffer of the folder with `realloc`, to the size of the file and one byte more.
Thus a folder keeps one buffer, and its next read uses it again.
If `realloc` fails, the old buffer stays in the folder, and `os_folder_close` frees it.

`fread` reads at most the size. If `ferror` then tells of an error, the read fails.
A file that became shorter after its size was read gives the bytes that it still has.
The byte after the bytes of the file is zero, so a text can be read as a string.

## Functions

| Function | Result |
| --- | --- |
| `os_read_stream(file, size, kept, len)` | Reads at most `size` bytes of `file` into `*kept`, as *The buffer* tells, and sets `*len` to their count. Gives the bytes, or `nullptr`. |
