/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <stdlib.h>

#include "os.h"

static int copy_file(const char *from, const char *to)
{
    FILE *in = fopen(from, "rb");
    FILE *out = fopen(to, "wb");
    char block[4096];
    size_t got = 1;
    int failed = !in || !out;

    while (!failed && got) {
        got = fread(block, 1, sizeof(block), in);
        failed = fwrite(block, 1, got, out) != got;
    }
    if (in)
        fclose(in);
    if (out && fclose(out))
        failed = 1;
    return (failed);
}

static int pin_copy(const char *path, bool closed)
{
    void *lib;
    int pinned;

    if (copy_file(FIXTURE_PATH, path))
        return (printf("FAIL: copy the fixture\n"), -2);
    lib = os_lib_open(path);
    if (!lib)
        return (printf("FAIL: open a copy of the fixture\n"), -2);
    if (closed)
        os_lib_close(lib);
    pinned = os_lib_pin(path);
    if (!closed)
        os_lib_close(lib);
    return (pinned);
}

int main(int argc, char **argv)
{
    char path[4096];
    int count;
    int pinned = 0;
    int result;

    if (argc < 3 || argc > 4)
        return (printf("usage: pins <folder> <count> [closed]\n"), 2);
    count = atoi(argv[2]);
    for (int i = 0; i < count; i++) {
        snprintf(path, sizeof(path), "%s/pin_%d%s", argv[1], i, BUILT_SUFFIX);
        result = pin_copy(path, argc == 4);
        if (result == -2)
            return (1);
        pinned += !result;
    }
    printf("pinned %d of %d libraries\n", pinned, count);
    return (0);
}
