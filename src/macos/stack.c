/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <pthread.h>

#include "os.h"

size_t os_thread_stack_left(void)
{
    pthread_t self = pthread_self();
    uintptr_t top = (uintptr_t)pthread_get_stackaddr_np(self);
    size_t size = pthread_get_stacksize_np(self);
    char here = 0;

    return ((size_t)((uintptr_t)&here - (top - size)));
}
