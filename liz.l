
/*-----------------------------------------------------------------------
    A flex specification for Wiz.
    For use in the COMP90045 project 2014.

    Team:       Master Minds
    Members:    629204 Ye He
                547108 Yi Xia
                645452 Rongzuo Liu 
-----------------------------------------------------------------------*/


%option noyywrap
%option nounput
%option noinput
%x COMMENT



DIGIT   [0-9]
INT     -?{DIGIT}+
FLOAT   -?{DIGIT}+\.{DIGIT}+
STRING  \"[^"\n\t]*\"
INTERVAL \.\.
ID      [a-zA-Z_][A-Za-z_0-9]*

nl      [\n\f]
nonl    [^\n\f]

%{

#include <stdlib.h>
#include <string.h>
#include "std.h"
#include "ast.h"
#include "piz.h"

int yylex(void);
extern int ln;

%}

%%

"#"           { BEGIN(COMMENT); }
<COMMENT>{nl} { BEGIN(INITIAL); }
<COMMENT>.    { }

":" | 
"," | 
";" | 
"(" | 
")" | 
"[" | 
"]" | 
"+" | 
"-" | 
"*" | 
"/" |
"=" |
"<" |
">"               { return yytext[0]; }

"true"            { return TRUE_TOKEN; }
"false"           { return FALSE_TOKEN; }
"do"              { return DO_TOKEN; }
"while"           { return WHILE_TOKEN; }
"end"             { return END_TOKEN; }
"if"              { return IF_TOKEN; }
"then"            { return THEN_TOKEN; }
"else"            { return ELSE_TOKEN; }
"fi"              { return FI_TOKEN; }
"or"              { return OR_TOKEN; }
"not"             { return NOT_TOKEN; }
"and"             { return AND_TOKEN; }
"int"             { return INT_TOKEN; }
"bool"            { return BOOL_TOKEN; }
"float"           { return FLOAT_TOKEN; }
"od"              { return OD_TOKEN; }
"proc"            { return PROC_TOKEN; }
"val"             { return VAL_TOKEN; }
"ref"             { return REF_TOKEN; }
"read"            { return READ_TOKEN; }
"write"           { return WRITE_TOKEN; }

{INT}             { yylval.str_val = (char *) strdup(yytext); return I_CONSTANT; }
{FLOAT}           { yylval.str_val = (char *) strdup(yytext); return F_CONSTANT; }
{STRING}          { yylval.str_val = (char *) strdup(yytext); return STRING_LITERAL; }

{ID}              { yylval.str_val = (char *) strdup(yytext); return IDENT_TOKEN; }

{INTERVAL}        { return INTERVAL_TOKEN; }

"<="          { return LE_OP; }
">="          { return GE_OP; }
"!="          { return NE_OP; }

":="            { return ASSIGN_TOKEN; }

"#"{nonl}*{nl}  { ln++; }

{nl}            { ln++; }

[ \t]+          ; /* skip whitespace */

.               { return INVALID_TOKEN; }

%%

