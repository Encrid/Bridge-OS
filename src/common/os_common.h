/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef BRIDGE_OS_COMMON_H
    #define BRIDGE_OS_COMMON_H

    #include <stdio.h>

char *os_read_stream(FILE *file, size_t size, char **kept, size_t *len);

#endif
