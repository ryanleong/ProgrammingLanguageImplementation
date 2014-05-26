
/*-----------------------------------------------------------------------
	Functions for constructing abstract syntax trees nodes.
	For use the COMP90045 project 2014.

    Team:       Bandicoot 
    Members:    Wee Lee Tan     377990
                Rui Cheng       664000
                Ryan Leong      395463

	Modified from skeleton code of
    Team:       Master Minds
 -----------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "ast.h"
#include "std.h"

static int str_to_int(const char *);

const char *binop_names[] = { BINOP_NAMES };
const char *unop_names[] = { UNOP_NAMES };
const char *relop_names[] = { RELATION_OP_NAMES };

Expr new_id(char *str_val) {
    Expr id = allocate(sizeof(struct expr));
    id->kind = EXPR_ID;
    id->id = str_val;
    id->e1 = NULL;
    id->e2 = NULL;
    return id;
}

Expr new_integer(const char *str_val) {
    Expr num = allocate(sizeof(struct expr));
    num->kind = EXPR_CONST;
    
    num->constant.type = INT_CONSTANT;
    num->constant.val.int_val = str_to_int(str_val);
    return num;
}

Expr new_float(const char *str_val) {
	errno = 0;
	Expr num = allocate(sizeof(struct expr));
	num->kind = EXPR_CONST;
	    
	num->constant.type = FLOAT_CONSTANT;
	num->constant.val.float_val = (char *) strdup(str_val);
	return num;
}

Expr new_bool(BOOL val, int ln) {
	Expr bool_constant = allocate(sizeof(struct expr));
	bool_constant->kind = EXPR_CONST;
	bool_constant->constant.val.bool_val = val;
	bool_constant->constant.type = BOOL_CONSTANT;
	bool_constant->e1 = NULL;
	bool_constant->e2 = NULL;
	bool_constant->lineno = ln;
	return bool_constant;
}

Expr new_string(char *str_val) {
    Expr string = allocate(sizeof(struct expr));
    string->kind = EXPR_CONST;
    string->constant.val.str_val = str_val;
    string->constant.type = STRING_CONSTANT;
    string->e1 = NULL;
    string->e2 = NULL;
    return string;
}

Expr new_array(char *id, Exprs exprs) {
	Expr arr = allocate(sizeof(struct expr));
	arr->kind = EXPR_ARRAY;
	arr->id = id;
	arr->es = exprs;
	return arr;
}


Expr new_relop(Expr e1, RelOp relop, Expr e2) {
    Expr rel_expr = allocate(sizeof(struct expr));
    rel_expr->kind = EXPR_RELOP;
    rel_expr->relop = relop;
    rel_expr->e1 = e1;
    rel_expr->e2 = e2;
    return rel_expr;
}

Expr new_binaryop(BinOp binop, Expr e1, Expr e2, int ln) {
	Expr binop_expr = allocate(sizeof(struct expr));
	binop_expr->kind = EXPR_BINOP;
	binop_expr->binop = binop;
	binop_expr->e1 = e1;
	binop_expr->e2 = e2;
	binop_expr->lineno = ln;
	return binop_expr;
}

Expr new_unaryop(UnOp unop, Expr e1, int ln) {
	Expr unop_expr = allocate(sizeof(struct expr));
	unop_expr->kind = EXPR_UNOP;
	unop_expr->unop = unop;
	unop_expr->e1 = e1;
	unop_expr->e2 = NULL;
	unop_expr->lineno = ln;
	return unop_expr;
}


Exprs new_exprs(Expr first, Exprs rest) {
	Exprs expressions = allocate(sizeof(struct exprs));
	expressions->first = first;
	expressions->rest = rest;
	return expressions;	
}

Decls new_decls(Decl first, Decls rest) {
	Decls decls = allocate(sizeof(struct decls));
	decls->first = first;
	decls->rest = rest;
	return decls;
}

Decl new_decl(Type type, char *id, int ln) {
	Decl decl = allocate(sizeof(struct decl));
	decl->type = type;
	decl->id = id;
	decl->lineno = ln;
	return decl;
}

Decl new_array_decl(Type type, char *id, Intervals intervals, int ln) {
	Decl decl = allocate(sizeof(struct decl));
	decl->type = type;
	decl->id = id;
	decl->intervals = intervals;
	decl->lineno = ln;
	return decl;
}


Stmts new_stmts(Stmt first, Stmts rest) {
    Stmts stmts = allocate(sizeof(struct stmts));
    stmts->first = first;
    stmts->rest = rest;
    return stmts;
}

Stmt new_assign(char *id, Expr assign_expr, int ln) {
	Stmt assign_stmt = allocate(sizeof(struct stmt));
	assign_stmt->kind = STMT_ASSIGN;
	assign_stmt->info.assign.id = id;
	assign_stmt->info.assign.expr = assign_expr;
	assign_stmt->lineno = ln;
	return assign_stmt;
}

Stmt new_assign_array(char *id, Exprs exprs, Expr assign_expr, int ln) {
	Stmt assign_array = allocate(sizeof(struct stmt));
	assign_array->kind = STMT_ASSIGN_ARRAY;
	assign_array->info.assign.id = id;
	assign_array->info.assign.expr = assign_expr;
	assign_array->info.assign.exprs = exprs;
	assign_array->lineno = ln;
	return assign_array;
}

Stmt new_cond(Expr cond, Stmts then_branch, Stmts else_branch, int ln) {
	Stmt cond_stmt = allocate(sizeof(struct stmt));
	cond_stmt->kind = STMT_COND;
	cond_stmt->info.cond.cond = cond;
	cond_stmt->info.cond.then_branch = then_branch;
	cond_stmt->info.cond.else_branch = else_branch;
	cond_stmt->lineno = ln;
	return cond_stmt;
}

Stmt new_while(Expr cond, Stmts body, int ln) {
	Stmt while_stmt = allocate(sizeof(struct stmt));
	while_stmt->kind = STMT_WHILE;
	while_stmt->info.loop.cond = cond;
	while_stmt->info.loop.body = body;
	while_stmt->lineno = ln;
	return while_stmt;
}

Stmt new_read(char *id, int ln) {
	Stmt read_stmt = allocate(sizeof(struct stmt));
	read_stmt->kind = STMT_READ;
	read_stmt->info.read = id;
	read_stmt->lineno = ln;
	return read_stmt;
}

Stmt new_read_array(char *id, int ln, Exprs exprs) {
	Stmt read_array = allocate(sizeof(struct stmt));
	read_array->kind = STMT_READ_ARRAY;
	read_array->info.array.id = id;
	read_array->info.array.exprs = exprs;
	read_array->lineno = ln;
	return read_array;
}

Stmt new_write(Expr expr, int ln) {
	Stmt write_stmt = allocate(sizeof(struct stmt));
	write_stmt->kind = STMT_WRITE;
	write_stmt->info.write = expr;
	write_stmt->lineno = ln;
	return write_stmt;
}

Params new_params(Param first, Params rest) {
	Params params = allocate(sizeof(struct params));
	params->first = first;
	params->rest = rest;
	return params;
}

Param new_param(ParamKind kind, ParamType type, char *name) {
	Param param = allocate(sizeof(struct param));
	param->kind = kind;
	param->type = type;
	param->id = name;
	return param;
}

Header new_header(char *id, Params params) {
	Header header = allocate(sizeof(struct header));
	header->id = id;
	header->params = params;
	return header;
}

Procs new_procs(Proc first, Procs rest) {
	Procs procs = allocate(sizeof(struct procs));
	procs->first = first;
	procs->rest = rest;
	return procs;
}

Proc new_proc(Header header, Decls decls, Stmts stmts) {
	Proc proc = allocate(sizeof(struct proc));
	proc->header = header;
	proc->decls = decls;
	proc->body = stmts;
	return proc;
}

Stmt new_fncall(char *id, Exprs args) {
	Stmt fncall = allocate(sizeof(struct stmt));
	fncall->kind = STMT_FNCALL;
	fncall->info.fncall.id = id;
	fncall->info.fncall.args = args;
	return fncall;
}

Intervals new_intervals(Interval first, Intervals rest) {
	Intervals intervs = allocate(sizeof(struct intervals));
	intervs->first = first;
	intervs->rest = rest;
	return intervs;
}

Interval new_interval(char *start, char *end) {
	int start_num = str_to_int(start);
	int end_num = str_to_int(end);
	Interval interv = allocate(sizeof(struct interval));
	interv->start = start_num;
	interv->end = end_num;
	return interv;
}


static int str_to_int(const char *str_val) {
    errno = 0;
    int int_val = strtol(str_val, NULL, 10);
    if (errno != 0) {
        char error_msg[50] = "Cant't new integer: ";
        report_error_and_exit(strcat(error_msg, str_val));
    }
    return int_val;
}


