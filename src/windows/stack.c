/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "os_types.h"

constexpr ULONG LEAST_GUARANTEE = 8192;
constexpr ULONG_PTR GUARD = 8192;

size_t os_thread_stack_left(void)
{
    ULONG_PTR low = 0;
    ULONG_PTR high = 0;
    ULONG guarantee = 0;
    char here = 0;

    GetCurrentThreadStackLimits(&low, &high);
    SetThreadStackGuarantee(&guarantee);
    if (guarantee < LEAST_GUARANTEE)
        guarantee = LEAST_GUARANTEE;
    return ((size_t)((ULONG_PTR)&here - low - guarantee - GUARD));
}
