/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>

#include "os.h"

static void fill_output(void)
{
    while (!os_signal_is_caught())
        printf("the output is full\n");
}

int main(int argc, char **argv)
{
    static char said[128];
    char line[16];

    if (argc < 2 || argc > 3)
        return (printf("usage: signal <line> [fill]\n"), 2);
    snprintf(said, sizeof(said), "%s\n", argv[1]);
    if (os_signal_catch(said))
        return (printf("FAIL: catch the signals\n"), 1);
    printf("ready\n");
    fflush(stdout);
    if (argc == 3)
        fill_output();
    else if (!fgets(line, sizeof(line), stdin))
        return (1);
    printf("%s\n", os_signal_is_caught() ? "caught" : "not caught");
    fflush(stdout);
    if (fgets(line, sizeof(line), stdin))
        printf("stopped\n");
    return (0);
}
