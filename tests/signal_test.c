/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>

#include "os.h"

int main(void)
{
    os_event_t *pause = os_event_create();
    char line[16];

    if (!pause || os_signal_catch())
        return (printf("FAIL: catch the signals\n"), 1);
    printf("ready\n");
    fflush(stdout);
    while (!os_signal_is_caught())
        os_event_wait(pause, OS_SECOND / 100);
    printf("caught\n");
    fflush(stdout);
    if (fgets(line, sizeof(line), stdin))
        printf("stopped\n");
    os_event_free(pause);
    return (0);
}
