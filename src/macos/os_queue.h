/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef BRIDGE_OS_QUEUE_H
    #define BRIDGE_OS_QUEUE_H

void *os_queue_make(void);

void os_queue_wait(void *queue);

void os_queue_free(void *queue);

#endif
