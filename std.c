
/*-----------------------------------------------------------------------
    Definitions for the utilities for Wiz programs.
    For use in the COMP90045 project 2014.

    Team:       Master Minds
    Members:    629204 Ye He
                547108 Yi Xia
                645452 Rongzuo Liu 
 -----------------------------------------------------------------------*/

#include "std.h"
#include <stdlib.h>
#include <stdio.h>


void *allocate(int size) {
    void *addr = malloc((size_t) size);
    if (addr == NULL)
        report_error_and_exit("Out of memory");
    return addr;
}

void report_error_and_exit(const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(EXIT_FAILURE);
}

