/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>

#include "os.h"
#include "counted.h"

static void noop(void *arg)
{
    (void)arg;
}

static void arm(size_t at)
{
    counted_t *c = counted_state();

    c->asked = 0;
    c->fail_at = at;
}

static int drive_event(void)
{
    for (size_t at = 1; at <= 8; at++) {
        os_event_t *e;

        arm(at);
        e = os_event_create();
        if (e) {
            os_event_free(e);
            return (0);
        }
    }
    return (printf("FAIL: event always refused\n"), 1);
}

static int drive_mutex(void)
{
    for (size_t at = 1; at <= 8; at++) {
        os_mutex_t *m;

        arm(at);
        m = os_mutex_create();
        if (m) {
            os_mutex_free(m);
            return (0);
        }
    }
    return (printf("FAIL: mutex always refused\n"), 1);
}

static int drive_thread(void)
{
    for (size_t at = 1; at <= 8; at++) {
        os_thread_t *t;

        arm(at);
        t = os_thread_start(noop, nullptr, 1 << 16);
        if (t) {
            os_thread_join(t);
            return (0);
        }
    }
    return (printf("FAIL: thread always refused\n"), 1);
}

int main(int argc, char **argv)
{
    (void)argv;
    if (argc > 1)
        return (1);
    if (drive_event() || drive_mutex() || drive_thread())
        return (1);
    printf("counted ok\n");
    return (0);
}
