/*-----------------------------------------------------------------------
    A driver for a pretty-printer for Wiz.
    For use the COMP90045 project 2014.
    See usage for details.

    Team:       Master Minds
    Members:    629204 Ye He
                547108 Yi Xia
                645452 Rongzuo Liu 
-----------------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "std.h"
#include "pretty.h"
#include "missing.h"

const char  *progname;
const char  *iz_infile;
Program parsed_program = NULL;

extern FILE *yyin;

static void usage(void);

int main(int argc, char **argv) {

    const char *in_filename;
    FILE *fp = stdout;
    BOOL pretty_print_only;

    progname = argv[0];
    pretty_print_only = FALSE;

    /* Process command line */

    if ((argc < 2) || (argc > 3)) {
        usage();
        exit(EXIT_FAILURE);
    }

    if (argc == 2)
        in_filename = argv[1];

    if (argc == 3 && streq(argv[1],"-p")) {
        pretty_print_only = TRUE;
        in_filename = argv[2];
    }
    
    iz_infile = in_filename;
    yyin = fopen(in_filename, "r");
    if (yyin == NULL) {
        perror(in_filename);
        exit(EXIT_FAILURE);
    }

    if (yyparse() != 0) {
        exit(EXIT_FAILURE);
    }

    if (pretty_print_only) 
        if (!parsed_program)
            report_error_and_exit("Empty file");
        else
            pretty_prog(fp, parsed_program);
    else
        report_error_and_exit("Unable to generate code");

    return 0;
}

static void usage(void) {
    printf("usage: wiz [-p] iz_source_file\n");
}

