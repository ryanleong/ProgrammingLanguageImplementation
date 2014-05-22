/*-----------------------------------------------------------------------
 Semantic analysis for the Wiz programs.
 For use in the COMP90045 project 2014.

    Team:      Bandicoot 
    Members:    
 -----------------------------------------------------------------------*/
#include "std.h"
#include "ast.h"

int analyze(FILE *, Program);


Type getExprType(Expr, char*);
void analyze_procs(Procs);
void analyze_proc(Proc);
void analyze_head(Proc);
void analyze_params(Params, char*, int);
void analyze_pram(Param, char*, int);
void analyze_decls(Decls, char*);
void analyze_decl(Decl, char*);
void analyze_stmts(Stmts, char*);
void analyze_stmt(Stmt, char*);
void analyze_assign(Assign, char*);
void analyze_assignArray(Assign, char*);
void analyze_cond(Cond, char*);
void analyze_while(While, char*);
void analyze_read(Stmt, char*);
void analyze_write(Stmt, char*);
void analyze_fncall(Stmt, char*);
void analyze_expr(Expr, char*);
void analyze_exprs(Exprs, char*);
int getArrDimension(Intervals);
int getArrSize(Intervals);
void analyze_args(Exprs, char*, char*, int);
void analyze_arg(Expr, char*, int, char*);
void analyze_procHeads(Procs);
Type getArrayType(Type);
