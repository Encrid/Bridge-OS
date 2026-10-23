/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef COUNTED_H
    #define COUNTED_H

    #include <stddef.h>

typedef struct {
    size_t asked;
    size_t fail_at;
} counted_t;

counted_t *counted_state(void);
bool counted_request(void);

void *counted_malloc(size_t n);
void *counted_calloc(size_t n, size_t each);
void *counted_realloc(void *p, size_t n);
void counted_free(void *p);

#endif
