Programming Language Implementation
====================================

With this README file you should also have found these:

    Makefile :  A makefile for the COMP90045 project 2014

    ast.c :     Defines Iz Abstract Syntax Trees in C; actually
                the bulk of definitions are in ast.h

    ast.h :     Many possible design choices - this is just one
                possible way of defining the data structures that
                make up the ast

    liz.l :     A lex specification for Iz
    
    missing.h : A header file that makes up for bison shortcomings

    piz.y :     A bison specification for Iz

    pretty.c :  A pretty-printer - actually not really; for now
                this is just a stub

    pretty.h :  A header file for pretty.c
    
    std.h :     A header file with some standard C macros
    
    wiz.c :     The driver program

To get started, study these files, in particular liz.l, ast.h,
and piz.y.  On a Unix machine you should be able to just type

    make

and that should generate some files for you, including liz.c,
piz.c, piz.h, and the executable wiz. (There will also be a file
piz.output that may be useful for piz.y debugging later on.)

Write a small Iz test program, like this:

    int k; read k;

Say this program is in file test.iz; now you should be able to run

    wiz -p test.iz

and something will happen (actually nothing very interesting,
because there is no real pretty-printer yet).  But at least
you should not get error messages.

If your Unix system doesn't seem to recognise `wiz', that
could be because your PATH variable hasn't been set correctly.
For now, just try instead

    ./wiz -p test.iz
