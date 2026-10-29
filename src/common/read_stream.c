/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdlib.h>

#include "os_common.h"

char *os_read_stream(FILE *file, size_t size, char **kept, size_t *len)
{
    char *bytes = realloc(*kept, size + 1);
    size_t got = 0;

    if (!bytes)
        return (nullptr);
    *kept = bytes;
    got = fread(bytes, 1, size, file);
    if (ferror(file))
        return (nullptr);
    bytes[got] = '\0';
    *len = got;
    return (bytes);
}
