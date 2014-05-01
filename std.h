
/*-----------------------------------------------------------------------
    Declarations for the utilities for Wiz programs.
    For use in the COMP90045 project 2014.

    Team:       Master Minds
    Members:    629204 Ye He
                547108 Yi Xia
                645452 Rongzuo Liu 
 -----------------------------------------------------------------------*/

#ifndef STD_H
#define STD_H

#define max(a, b) ((a) < (b) ? (b) : (a))
#define max3(a, b, c) (max((a), max((b), (c))))

#define streq(s1, s2) (strcmp((s1), (s2)) == 0)

#ifndef NULL
#define NULL 0
#endif
#ifndef NORMAL_EXIT
#define NORMAL_EXIT 0
#endif
#ifndef FALSE
#define FALSE ((BOOL) 0)
#endif
#ifndef TRUE
#define TRUE  ((BOOL) 1)
#endif
#ifndef BOOL
#define BOOL char
#endif

void *allocate(int size);
void report_error_and_exit(const char *msg);

#endif  /* STD_H */
