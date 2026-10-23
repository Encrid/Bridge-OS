/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdlib.h>

#include "counted.h"

static counted_t state;

counted_t *counted_state(void)
{
    return (&state);
}

bool counted_request(void)
{
    state.asked++;
    return (state.asked == state.fail_at);
}

void *counted_malloc(size_t n)
{
    if (counted_request())
        return (nullptr);
    return (malloc(n));
}

void *counted_calloc(size_t n, size_t each)
{
    if (counted_request())
        return (nullptr);
    return (calloc(n, each));
}

void *counted_realloc(void *p, size_t n)
{
    if (counted_request())
        return (nullptr);
    return (realloc(p, n));
}

void counted_free(void *p)
{
    free(p);
}
