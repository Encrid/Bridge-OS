/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "os_watch.h"

constexpr char CORE[] =
    "/System/Library/Frameworks/CoreServices.framework/CoreServices";
constexpr char NAMES[CORE_CALLS][48] = {
    "CFStringCreateWithFileSystemRepresentation", "CFArrayCreate",
    "kCFTypeArrayCallBacks", "CFRelease", "FSEventStreamCreate",
    "FSEventStreamSetDispatchQueue", "FSEventStreamStart",
    "FSEventStreamStop", "FSEventStreamInvalidate", "FSEventStreamRelease",
};

bool os_core_load(os_watch_t *w)
{
    bool found = true;

    w->lib = os_lib_open(CORE);
    for (uint32_t i = 0; w->lib && found && i < CORE_CALLS; i++) {
        w->calls[i] = os_lib_get_symbol(w->lib, NAMES[i]);
        found = w->calls[i] != nullptr;
    }
    return (w->lib && found);
}

const void *os_core_string(os_watch_t *w)
{
    return (((os_string_fn_t *)w->calls[CORE_STRING])(nullptr, w->real));
}

const void *os_core_array(os_watch_t *w, const void *real)
{
    return (((os_array_fn_t *)w->calls[CORE_ARRAY])(nullptr, &real, 1,
        w->calls[CORE_CALLBACKS]));
}

void *os_core_create(os_watch_t *w, const void *paths,
    os_events_fn_t *on_events)
{
    os_context_t context = { .info = w };

    return (((os_create_fn_t *)w->calls[CORE_CREATE])(nullptr, on_events,
        &context, paths, OS_SINCE_NOW, OS_LATENCY, OS_STREAM_FLAGS));
}

bool os_core_start(os_watch_t *w)
{
    w->started = ((os_start_fn_t *)w->calls[CORE_START])(w->stream);
    return (w->started);
}
