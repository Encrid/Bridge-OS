/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "os.h"

static void set_flag(void *arg)
{
    *(int *)arg = 7;
}

static void set_later(void *arg)
{
    os_event_t *pause = os_event_create();

    if (pause)
        os_event_wait(pause, OS_SECOND / 20);
    os_event_free(pause);
    os_event_set(arg);
}

static int wait_for_thread(void)
{
    os_event_t *e = os_event_create();
    os_thread_t *t;

    if (!e)
        return (printf("FAIL: event to wait for\n"), 1);
    t = os_thread_start(set_later, e);
    if (!t)
        return (printf("FAIL: thread that sets\n"), 1);
    os_event_wait(e, OS_FOREVER);
    os_thread_join(t);
    os_event_free(e);
    return (0);
}

int main(int argc, char **argv)
{
    const char *dir;
    char path[4096];
    char longname[300];
    void *base;
    os_event_t *e;
    os_thread_t *t;
    os_mutex_t *m;
    char *whole;
    void *lib;
    void *sym;
    int (*answer)(void);
    int flag = 0;

    if (argc < 2)
        return (printf("FAIL: no workdir\n"), 1);
    if (argc > 2)
        return (1);
    dir = argv[1];

    base = os_reserve(8192, 4096);
    if (!base)
        return (printf("FAIL: reserve\n"), 1);
    if (os_commit(base, 8192))
        return (printf("FAIL: commit\n"), 1);
    os_release(base, 8192);
    if (os_reserve((size_t)-1, 4096))
        return (printf("FAIL: reserve past the address space\n"), 1);
    if (os_reserve(4096, (size_t)-1))
        return (printf("FAIL: commit past the reservation\n"), 1);
    base = os_reserve(8192, 4096);
    if (!base)
        return (printf("FAIL: reserve again\n"), 1);
    if (!os_commit(base, (size_t)-1))
        return (printf("FAIL: commit a bad size\n"), 1);
    os_release(base, 8192);

    e = os_event_create();
    if (!e)
        return (printf("FAIL: event\n"), 1);
    os_event_wait(e, OS_SECOND / 1000);
    os_event_set(e);
    os_event_wait(e, OS_SECOND / 1000);
    os_event_set(e);
    os_event_wait(e, OS_FOREVER);
    os_event_free(e);
    os_event_free(nullptr);
    if (wait_for_thread())
        return (1);

    t = os_thread_start(set_flag, &flag);
    if (!t)
        return (printf("FAIL: thread\n"), 1);
    os_thread_join(t);
    if (flag != 7)
        return (printf("FAIL: thread work\n"), 1);
    if (!os_get_thread_id())
        return (printf("FAIL: thread id\n"), 1);
    (void)os_get_now();

    m = os_mutex_create();
    if (!m)
        return (printf("FAIL: mutex\n"), 1);
    os_mutex_lock(m);
    os_mutex_unlock(m);
    os_mutex_free(m);
    os_mutex_free(nullptr);

    snprintf(path, sizeof(path), "%s/lock.tmp", dir);
    if (os_lock(path))
        return (printf("FAIL: lock\n"), 1);
    if (os_lock(path) != 1)
        return (printf("FAIL: relock\n"), 1);
    if (os_lock("nowhere_dir/lock.tmp") != -1)
        return (printf("FAIL: lock a bad path\n"), 1);

    whole = os_path_whole(".");
    if (!whole)
        return (printf("FAIL: path\n"), 1);
    free(whole);

    snprintf(path, sizeof(path), "%s/new_folder", dir);
    if (os_folder_make(path))
        return (printf("FAIL: make a folder\n"), 1);
    if (os_folder_make(path))
        return (printf("FAIL: make an existing folder\n"), 1);
    snprintf(path, sizeof(path), "%s/lock.tmp", dir);
    if (os_folder_make(path) != -1)
        return (printf("FAIL: make a folder over a file\n"), 1);
    if (os_folder_set("."))
        return (printf("FAIL: set the folder\n"), 1);

    memset(longname, 'a', sizeof(longname) - 1);
    longname[sizeof(longname) - 1] = '\0';
    if (os_lib_open(longname))
        return (printf("FAIL: open a long name\n"), 1);
    if (os_lib_open("no_such_library"))
        return (printf("FAIL: open a bare name\n"), 1);
    if (os_lib_open("no/such/library"))
        return (printf("FAIL: open a path with a slash\n"), 1);
    if (os_lib_open("no\\such\\library"))
        return (printf("FAIL: open a path with a backslash\n"), 1);
    lib = os_lib_open(FIXTURE_PATH);
    if (!lib)
        return (printf("FAIL: open the fixture\n"), 1);
    sym = os_lib_get_symbol(lib, "fixture_answer");
    if (!sym)
        return (printf("FAIL: find the symbol\n"), 1);
    memcpy(&answer, &sym, sizeof(answer));
    if (answer() != 42)
        return (printf("FAIL: call the symbol\n"), 1);
    os_lib_close(lib);

    printf("os ok\n");
    return (0);
}
