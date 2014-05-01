
/*-----------------------------------------------------------------------
    Declarations for the abstract syntax trees generated for Wiz programs.
    For use in the COMP90045 project 2014.

    Team:       Master Minds
    Members:    629204 Ye He
                547108 Yi Xia
                645452 Rongzuo Liu 
 -----------------------------------------------------------------------*/

#ifndef AST_H
#define AST_H

#include "std.h"

typedef struct decl   *Decl;
typedef struct decls  *Decls;
typedef struct expr   *Expr;
typedef struct exprs  *Exprs;
typedef struct stmts  *Stmts;
typedef struct stmt   *Stmt;
typedef struct proc   *Proc;
typedef struct procs  *Procs;
typedef struct header *Header;
typedef struct params *Params;
typedef struct param  *Param;
typedef struct prog   *Program;
typedef struct intervals   *Intervals;
typedef struct interval   *Interval;



typedef enum {
    BINOP_ADD, BINOP_SUB, BINOP_MUL, BINOP_DIV,
    BINOP_AND, BINOP_OR
} BinOp;

#define BINOP_NAMES "+", "-", "*", "/", "and", "or"

extern const char *binop_names[];


typedef enum {
    UNOP_MINUS, UNOP_NOT
} UnOp;

#define UNOP_NAMES "-", "not"

extern const char *unop_names[];


typedef enum {
    RELOP_EQ, RELOP_NE, RELOP_LT, RELOP_GT, RELOP_LE, RELOP_GE
} RelOp;

#define RELATION_OP_NAMES "=", "!=", "<", ">", "<=", ">="

extern const char *relop_names[];


typedef enum {
    BOOL_TYPE, INT_TYPE, FLOAT_TYPE, STRING_TYPE,
    INT_ARRAY_TYPE, FLOAT_ARRAY_TYPE, BOOL_ARRAY_TYPE
} Type;

typedef union {
    int    int_val;
    BOOL   bool_val;
    char   *float_val;
    char   *str_val;
} Value;

typedef enum {
    BOOL_CONSTANT, INT_CONSTANT, FLOAT_CONSTANT, STRING_CONSTANT
} ConstantType;

typedef struct {
    ConstantType   type;
    Value  val;
} Constant;

typedef enum {
    EXPR_ID, EXPR_ARRAY, EXPR_CONST, EXPR_BINOP, EXPR_UNOP, EXPR_RELOP
} ExprKind;

struct expr {
    int       lineno;
    ExprKind  kind;
    char      *id;          /* for identifiers */
    Constant  constant;     /* for constant values */
    RelOp     relop;        /* for relational operators */
    UnOp      unop;         /* for unary operators */
    BinOp     binop;        /* for binary operators */
    Expr      e1;           /* for unary and binary operators */
    Expr      e2;           /* for binary operators */
    Exprs     es;           /* for array operator */
};

struct exprs {
    Expr first;
    Exprs rest;
};

struct decl {
    int       lineno;
    char      *id;
    Type      type;
    Intervals intervals;    /* for array declaration */
};

struct decls {
    Decl      first;
    Decls     rest;
};

typedef struct {
    char      *id;
    Expr      expr;
    Exprs     exprs;
} Assign;

typedef struct {
    Expr      cond;
    Stmts     then_branch;
    Stmts     else_branch;
} Cond;

typedef struct {
    Expr      cond;
    Stmts     body;
} While;

typedef struct {
    char    *id;
    Exprs   args;
} Fncall;

typedef struct {
    char    *id;
    Exprs   exprs;
} Array;

typedef enum {
    STMT_ASSIGN, STMT_COND, STMT_READ, STMT_WHILE, STMT_WRITE,
    STMT_FNCALL, STMT_READ_ARRAY, STMT_ASSIGN_ARRAY
} StmtKind;

typedef union {
    Fncall    fncall;
    Assign    assign;
    Stmts     stmts;
    Cond      cond;
    char      *read;
    Expr      write;
    While     loop;
    Array     array;    /* for read <id> [ <expr_list> ] ; */
} SInfo;

struct stmt {
    int       lineno;
    StmtKind  kind;
    SInfo     info;
};

struct stmts {
    Stmt      first;
    Stmts     rest;
};

struct proc {
    Header    header;
    Decls     decls;
    Stmts     body;
};

struct header {
    char      *id;
    Params    params;
};

struct params {
    Param      first;
    Params     rest;
};

typedef enum {
    REF, VAL
} ParamKind;


typedef enum {
    BOOL_PARAM, INT_PARAM, FLOAT_PARAM
} ParamType;

struct param {
    char      *id;
    ParamKind kind;
    ParamType type;
};

struct procs {
    Proc      first;
    Procs     rest;
};

struct prog {
    Procs     procs;
};

struct interval {
    int start;
    int end;
};

struct intervals {
    Interval first;
    Intervals rest;
};


Expr new_id(char *);
Expr new_integer(const char *);
Expr new_float(const char *);
Expr new_bool(BOOL, int);
Expr new_string(char *);
Expr new_array(char *, Exprs);
Expr new_relop(Expr, RelOp, Expr);
Expr new_binaryop(BinOp, Expr, Expr, int);
Expr new_unaryop(UnOp, Expr, int);
Exprs new_exprs(Expr, Exprs);
Decl new_decl(Type, char *, int);
Decl new_array_decl(Type, char *, Intervals, int);
Decls new_decls(Decl, Decls);
Stmts new_stmts(Stmt, Stmts);
Stmt new_assign(char *, Expr, int);
Stmt new_assign_array(char *, Exprs, Expr, int);
Stmt new_cond(Expr, Stmts, Stmts, int);
Stmt new_while(Expr, Stmts, int);
Stmt new_read(char *, int);
Stmt new_read_array(char *, int, Exprs);
Stmt new_write(Expr, int);
Params new_params(Param, Params);
Param new_param(ParamKind, ParamType, char *);
Header new_header(char *, Params);
Proc new_proc(Header, Decls, Stmts);
Procs new_procs(Proc, Procs);
Stmt new_fncall(char *, Exprs);
Intervals new_intervals(Interval, Intervals);
Interval new_interval(char *, char *);



#endif /* AST_H */
