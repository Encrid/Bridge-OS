/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <string.h>

#include "os_common.h"

bool os_is_within(const char *name, uint32_t depth)
{
    uint32_t parts = 1;

    for (const char *at = strchr(name, '/'); at; at = strchr(at + 1, '/'))
        parts++;
    return (parts <= depth);
}
