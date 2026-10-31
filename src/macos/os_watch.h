/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef BRIDGE_OS_WATCH_H
    #define BRIDGE_OS_WATCH_H

    #include "os_queue.h"
    #include "../posix/os_types.h"

constexpr size_t OS_NAMES_ROOM = 65536;
constexpr uint32_t OS_EVENTS_LOST = 0x00000001;
constexpr uint32_t OS_EVENTS_ENDED = 0x00000020 | 0x00000080;
constexpr uint32_t OS_STREAM_FLAGS = 0x00000002 | 0x00000004 | 0x00000010;
constexpr uint64_t OS_SINCE_NOW = UINT64_MAX;
constexpr double OS_LATENCY = 0.05;

enum os_core : uint32_t {
    CORE_STRING,
    CORE_ARRAY,
    CORE_CALLBACKS,
    CORE_RELEASE,
    CORE_CREATE,
    CORE_QUEUE,
    CORE_START,
    CORE_STOP,
    CORE_INVALIDATE,
    CORE_FREE,
    CORE_CALLS,
};

typedef void os_events_fn_t(const void *stream, void *info, size_t count,
    void *paths, const uint32_t *flags, const uint64_t *ids);
typedef const void *os_string_fn_t(const void *allocator, const char *path);
typedef const void *os_array_fn_t(const void *allocator,
    const void **values, long count, const void *callbacks);
typedef void os_release_fn_t(const void *object);
typedef void *os_create_fn_t(const void *allocator, os_events_fn_t *callback,
    void *context, const void *paths, uint64_t since, double latency,
    uint32_t flags);
typedef void os_queue_fn_t(void *stream, void *queue);
typedef unsigned char os_start_fn_t(void *stream);
typedef void os_stream_fn_t(void *stream);

typedef struct os_names {
    size_t used;
    char bytes[OS_NAMES_ROOM];
} os_names_t;

typedef struct os_context {
    long version;
    void *info;
    void *retain;
    void *release;
    void *describe;
} os_context_t;

struct os_watch {
    void *lib;
    void *calls[CORE_CALLS];
    void *stream;
    void *queue;
    os_mutex_t *lock;
    char *real;
    size_t skip;
    uint32_t depth;
    bool started;
    bool lost;
    bool ended;
    size_t at;
    os_names_t *filling;
    os_names_t *reading;
    os_names_t names[2];
};

bool os_core_load(os_watch_t *w);

const void *os_core_string(os_watch_t *w);

const void *os_core_array(os_watch_t *w, const void *real);

void *os_core_create(os_watch_t *w, const void *paths,
    os_events_fn_t *on_events);

bool os_core_start(os_watch_t *w);

void os_core_queue(os_watch_t *w);

void os_core_release(os_watch_t *w, const void *object);

void os_core_end(os_watch_t *w);

bool os_open_stream(os_watch_t *w);

#endif
