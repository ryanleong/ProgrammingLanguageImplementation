
/*-----------------------------------------------------------------------
    A bison syntax spec for Wiz.
    For use in the COMP90045 project 2014.
    Harald Sondergaard, March 2014.

    Team:       Master Minds
    Members:    629204 Ye He
                547108 Yi Xia
                645452 Rongzuo Liu 
-----------------------------------------------------------------------*/

%{

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "std.h"
#include "missing.h"

extern Program parsed_program;
extern char    *yytext;

int ln = 1;
void yyerror(const char *msg);

%}

%union {
    int     int_val;
    char    *str_val;
    Interval interval_val;
    Intervals intervals_val;
    Decl    decl_val;
    Decls   decls_val;
    Procs   procs_val;
    Proc    proc_val;
    Params  params_val;
    Param   param_val;
    Expr    expr_val;
    Exprs   exprs_val;
    Stmts   stmts_val;
    Stmt    stmt_val;
    Header  header_val;
    Program prog_val;
    ParamKind indicator_val;
    ParamType param_type_val;
}

%token '(' ')' ';' '[' ']' ':' ',' '+' '-' '*' '/' '=' '<' '>'
%token ASSIGN_TOKEN 
%token DO_TOKEN   
%token OD_TOKEN    
%token IF_TOKEN    
%token THEN_TOKEN 
%token ELSE_TOKEN 
%token FI_TOKEN    
%token INT_TOKEN 
%token FLOAT_TOKEN
%token BOOL_TOKEN 
%token TRUE_TOKEN 
%token FALSE_TOKEN 
%token WHILE_TOKEN 
%token READ_TOKEN 
%token WRITE_TOKEN
%token END_TOKEN
%token PROC_TOKEN
%token VAL_TOKEN
%token REF_TOKEN
%token OR_TOKEN
%token NOT_TOKEN
%token AND_TOKEN
%token INTERVAL_TOKEN
%token INVALID_TOKEN

%token LE_OP
%token GE_OP
%token NE_OP

%token <str_val> IDENT_TOKEN
%token <str_val> I_CONSTANT
%token <str_val> F_CONSTANT
%token <str_val> STRING_LITERAL

/* Standard operator precedence */

%left OR_TOKEN
%left AND_TOKEN
%left NOT_TOKEN
%nonassoc '=' '<' '>' LE_OP GE_OP NE_OP
%left '+' '-' 
%left '*' '/'
%left UNARY_MINUS

%type <prog_val>  program
%type <decls_val> declarations
%type <decl_val>  decl
%type <stmts_val> statements 
%type <stmt_val>  stmt
%type <proc_val>  proc
%type <procs_val> procedures
%type <params_val> parameters
%type <params_val> nonempty_params
%type <param_val> param
%type <header_val> header
%type <indicator_val> indicator
%type <param_type_val> param_type
%type <expr_val> constant
%type <expr_val>  expr 
%type <exprs_val>  nonempty_exprs 
%type <exprs_val>  expressions 
%type <interval_val>  interval 
%type <intervals_val>  intervals 

%type <int_val>   assign
%type <int_val>   start_cond
%type <int_val>   start_read
%type <int_val>   start_while
%type <int_val>   start_write
%type <int_val>   get_lineno

%start program

%%
/*---------------------------------------------------------------------*/

program
    : procedures
        {
          parsed_program = allocate(sizeof(struct prog));
          parsed_program->procs = $1;
        }
    |
        { parsed_program = NULL; }
    ;

procedures
    : proc procedures
        { $$ = new_procs($1, $2); }

    | proc
        { $$ = new_procs($1, NULL); }
    ;    

proc 
    : PROC_TOKEN header declarations statements END_TOKEN
        { $$ = new_proc($2, $3, $4); }
    ;

header
    : IDENT_TOKEN '(' parameters ')'
        { $$ = new_header($1, $3); }
    ;

nonempty_params
    : param ',' nonempty_params
      { $$ = new_params($1, $3); }

    | param
      { $$ = new_params($1, NULL); }
    ;

parameters
    : nonempty_params
    | /* empty */
        { $$ = NULL; }
    ;

param_type 
    : INT_TOKEN     { $$ = INT_PARAM; }
    | FLOAT_TOKEN   { $$ = FLOAT_PARAM; }
    | BOOL_TOKEN    { $$ = BOOL_PARAM; }
    ;

indicator
    : REF_TOKEN     { $$ = REF; }
    | VAL_TOKEN     { $$ = VAL; }
    ;

param
    : indicator param_type IDENT_TOKEN 
        { $$ = new_param($1, $2, $3); }
    ;

declarations
    : decl declarations
        { $$ = new_decls($1, $2); }
    | /* empty */
        { $$ = NULL; }
    ;
        
decl
    : INT_TOKEN IDENT_TOKEN ';'
        { $$ = new_decl(INT_TYPE, $2, ln); }

    | FLOAT_TOKEN IDENT_TOKEN ';'
        { $$ = new_decl(FLOAT_TYPE, $2, ln); }
        
    | BOOL_TOKEN IDENT_TOKEN ';'
        { $$ = new_decl(BOOL_TYPE, $2, ln); }

    | INT_TOKEN IDENT_TOKEN '[' intervals ']' ';'
        { $$ = new_array_decl(INT_ARRAY_TYPE, $2, $4, ln); }

    | FLOAT_TOKEN IDENT_TOKEN '[' intervals ']' ';'
        { $$ = new_array_decl(FLOAT_ARRAY_TYPE, $2, $4, ln); }

    | BOOL_TOKEN IDENT_TOKEN '[' intervals ']' ';'
        { $$ = new_array_decl(BOOL_ARRAY_TYPE, $2, $4, ln); }
    ;

intervals
    : interval ',' intervals
        { $$ = new_intervals($1, $3); }

    | interval
        { $$ = new_intervals($1, NULL); }
    ;    

interval
    : I_CONSTANT INTERVAL_TOKEN I_CONSTANT
        { $$ = new_interval($1, $3); }

get_lineno
    :                { $$ = ln; }

assign
    : ASSIGN_TOKEN   { $$ = ln; }

start_cond
    : IF_TOKEN       { $$ = ln; }

start_read
    : READ_TOKEN     { $$ = ln; }

start_while
    : WHILE_TOKEN    { $$ = ln; }

start_write
    : WRITE_TOKEN    { $$ = ln; }

statements                             /* non-empty list of statements */
    : stmt statements
        { $$ = new_stmts($1, $2); }

    | stmt
        { $$ = new_stmts($1, NULL); }

    | error ';' { yyerrok; } statements
        { $$ = $4; }
    ;

stmt
    : IDENT_TOKEN assign expr ';'
        { $$ = new_assign($1, $3, $2); }

    | IDENT_TOKEN '[' expressions ']' assign expr ';'
        { $$ = new_assign_array($1, $3, $6, $5); }        

    | IDENT_TOKEN '(' expressions ')' ';'
        { $$ = new_fncall($1, $3); }

    | start_read IDENT_TOKEN ';'
        { $$ = new_read($2, $1); }

    | start_read IDENT_TOKEN '[' expressions ']' ';'
        { $$ = new_read_array($2, $1, $4); }

    | start_write expr ';'
        { $$ = new_write($2, $1); }

    | start_cond expr THEN_TOKEN statements FI_TOKEN 
        { $$ = new_cond($2, $4, NULL, $1); }

    | start_cond expr THEN_TOKEN statements ELSE_TOKEN statements FI_TOKEN
        { $$ = new_cond($2, $4, $6, $1); }

    | start_while expr DO_TOKEN statements OD_TOKEN
        { $$ = new_while($2, $4, $1); }
    ;

nonempty_exprs
    : expr ',' nonempty_exprs
      { $$ = new_exprs($1, $3); }

    | expr
      { $$ = new_exprs($1, NULL); }
    ;

expressions
    : nonempty_exprs
    | /* empty */
        { $$ = NULL; }
    ;


expr 
    : '-' get_lineno expr    %prec UNARY_MINUS
        { $$ = new_unaryop(UNOP_MINUS, $3, $2); }

    | NOT_TOKEN get_lineno expr
        { $$ = new_unaryop(UNOP_NOT, $3, $2); }

    | expr '+' get_lineno expr
        {
          int ln = $1->lineno == $4->lineno ? $1->lineno : $3;
          $$ = new_binaryop(BINOP_ADD, $1, $4, ln);
        }

    | expr '-' get_lineno expr
        {
          int ln = $1->lineno == $4->lineno ? $1->lineno : $3;
          $$ = new_binaryop(BINOP_SUB, $1, $4, ln);
        }

    | expr '*' get_lineno expr
        {
          int ln = $1->lineno == $4->lineno ? $1->lineno : $3;
          $$ = new_binaryop(BINOP_MUL, $1, $4, ln);
        }

    | expr '/' get_lineno expr
        { 
          int ln = $1->lineno == $4->lineno ? $1->lineno : $3;
          $$ = new_binaryop(BINOP_DIV, $1, $4, ln);
        }

    | expr AND_TOKEN get_lineno expr
        { 
          int ln = $1->lineno == $4->lineno ? $1->lineno : $3;
          $$ = new_binaryop(BINOP_AND, $1, $4, ln);
        }

    | expr OR_TOKEN get_lineno expr
        { 
          int ln = $1->lineno == $4->lineno ? $1->lineno : $3;
          $$ = new_binaryop(BINOP_OR, $1, $4, ln);
        }                

    | '(' expr ')'
        { $$ = $2; }

    | '(' error ')'
        { $$ = NULL; }

    | IDENT_TOKEN
        { $$ = new_id($1); }

    | IDENT_TOKEN '[' expressions ']'
        { $$ = new_array($1, $3); }
        
    | expr '<' expr     
        { $$ = new_relop($1, RELOP_LT, $3); }

    | expr '>' expr     
        { $$ = new_relop($1, RELOP_GT, $3); }

    | expr '=' expr     
        { $$ = new_relop($1, RELOP_EQ, $3); }

    | expr LE_OP expr   
        { $$ = new_relop($1, RELOP_LE, $3); }

    | expr GE_OP expr   
        { $$ = new_relop($1, RELOP_GE, $3); }

    | expr NE_OP expr   
        { $$ = new_relop($1, RELOP_NE, $3); }

    | constant
    ;

constant
    : I_CONSTANT        { $$ = new_integer($1); }
    | F_CONSTANT        { $$ = new_float($1); }
    | STRING_LITERAL    { $$ = new_string($1); }
    | TRUE_TOKEN        { $$ = new_bool(TRUE,ln); }
    | FALSE_TOKEN       { $$ = new_bool(FALSE,ln); }
    ;

%%

void 
yyerror(const char *msg) {

    fprintf(stderr, "**** Input line %d, near '%s': %s\n", ln, yytext, msg);
    return;
}
