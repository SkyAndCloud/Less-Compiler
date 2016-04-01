//
// Created by yongshan on 16-3-31.
//

#include <stdlib.h>
#include <stdio.h>
#include "util.h"

void myMalloc(void **p, size_t size)
{
    if ((*p = malloc(size)) == NULL)
    {
        fprintf(stderr, "\nNo available memory!\n\n");
        exit(EXIT_FAILURE);
    }
}


