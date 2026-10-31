/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "os_watch.h"

void os_core_queue(os_watch_t *w)
{
    ((os_queue_fn_t *)w->calls[CORE_QUEUE])(w->stream, w->queue);
}

void os_core_release(os_watch_t *w, const void *object)
{
    ((os_release_fn_t *)w->calls[CORE_RELEASE])(object);
}

void os_core_end(os_watch_t *w)
{
    if (w->started)
        ((os_stream_fn_t *)w->calls[CORE_STOP])(w->stream);
    ((os_stream_fn_t *)w->calls[CORE_INVALIDATE])(w->stream);
    ((os_stream_fn_t *)w->calls[CORE_FREE])(w->stream);
    os_queue_wait(w->queue);
}
