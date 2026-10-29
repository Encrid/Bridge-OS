/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <pthread.h>

#include "os.h"

static uintptr_t read_low(void)
{
    pthread_attr_t attr;
    void *low = nullptr;
    size_t size = 0;

    if (pthread_getattr_np(pthread_self(), &attr))
        return (0);
    pthread_attr_getstack(&attr, &low, &size);
    pthread_attr_destroy(&attr);
    return ((uintptr_t)low);
}

size_t os_thread_stack_left(void)
{
    static thread_local uintptr_t low;
    char here = 0;

    if (!low)
        low = read_low();
    if (!low)
        return (0);
    return ((size_t)((uintptr_t)&here - low));
}
