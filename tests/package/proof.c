/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>

#include "os.h"

int main(void)
{
    uint64_t now = os_get_now();

    printf("os package: %s\n", now ? "the clock runs" : "no clock");
    return (now ? 0 : 1);
}
