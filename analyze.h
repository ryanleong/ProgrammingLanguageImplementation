#include "std.h"
#include "ast.h"

int analyze(FILE *, Program);


Type getExprType(Expr, char*);
void analyze_procs(Procs);
void analyze_proc(Proc);
void analyze_head(Header);
void analyze_params(Params, char*, int);
void analyze_pram(Param, char*, int);
void analyze_decls(Decls, char*);
void analyze_decl(Decl, char*);
void analyze_stmts(Stmts, char*);
void analyze_stmt(Stmt, char*);
void analyze_assign(Assign, char*);
void analyze_assign_array(Assign, char*);
void analyze_cond(Cond, char*);
void analyze_while(While, char*);
void analyze_read(Stmt, char*);
void analyze_write(Stmt, char*);
void analyze_read_array(Stmt, char*);
void analyze_fncall(Stmt, char*);
void analyze_expr(Expr, char*);
Type getExprType(Expr, char*);
int getArrDimension(Intervals);
int getArrSize(Intervals);
void analyze_args(Exprs, char*, char*);
void analyze_arg(Expr, char*);