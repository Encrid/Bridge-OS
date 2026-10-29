/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <fcntl.h>

#include "../posix/os_types.h"

int os_open_search(int at, const char *name)
{
    return (openat(at, name, O_PATH | O_DIRECTORY | O_CLOEXEC));
}
