/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <dispatch/dispatch.h>

#include "os_queue.h"

static void wait_nothing([[maybe_unused]] void *context)
{
}

void *os_queue_make(void)
{
    return (dispatch_queue_create("bridge-os.watch", nullptr));
}

void os_queue_wait(void *queue)
{
    dispatch_sync_f(queue, nullptr, wait_nothing);
}

void os_queue_free(void *queue)
{
    dispatch_queue_t made = queue;

    dispatch_release(made);
}
