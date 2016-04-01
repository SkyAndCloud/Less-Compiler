//
// Created by yongshan on 16-3-31.
//

#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

struct item
{
    char *name;
    bool isVar;
    struct item *parent;
    struct item *next;

    union memory
    {
        struct var
        {
            char* value;
            char type;
        } varMemory;
        struct item *sonMemory;
    } mem;
};

void myMalloc(void **p, size_t size);

#endif
