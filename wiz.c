/*-----------------------------------------------------------------------
    A driver for a pretty-printer for Wiz.
    For use the COMP90045 project 2014.
    See usage for details.

    Team:       Bandicoot 
    Members:    Wee Lee Tan     377990
                Rui Cheng       664000
                Ryan Leong      395463
-----------------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "std.h"
#include "analyze.h"
#include "missing.h"
//#include "pretty.h"

const char  *progname;
const char  *iz_infile;
Program parsed_program = NULL;

extern FILE *yyin;

static void usage(void);

int main(int argc, char **argv) {

    const char *in_filename;
    FILE *fp = stdout;
    BOOL pretty_print_only;

    /* Process command line */

    if (argc != 2) {
        usage();
        exit(EXIT_FAILURE);
    }

    if (argc == 2)
        in_filename = argv[1];

    
    iz_infile = in_filename;
    yyin = fopen(in_filename, "r");
    if (yyin == NULL) {
        perror(in_filename);
        exit(EXIT_FAILURE);
    }

    if (yyparse() != 0) {
        exit(EXIT_FAILURE);
    }

    if (!parsed_program)
        report_error_and_exit("Empty file");
    else {
        if(analyze(fp, parsed_program)==0)
            codegen_prog(fp, parsed_program);
    }


    return 0;
}

static void usage(void) {
    printf("usage: wiz iz_source_file\n");
}

