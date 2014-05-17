#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "std.h"
#include "symbol.h"
#include "analyze.h"

int slotNum = 0;
int hasMain = 0;
int errorNum = 0;

int analyze(FILE *fp, Program prog)
{
	analyze_procs(prog->procs);
	if(hasMain==0)
	{
    	printf("No main procedure!\n");
    }
    return errorNum;
}

void analyze_procs(Procs procs)
{
    Proc first = procs->first;
    Procs rest = procs->rest;
    if(first)
    {
        analyze_proc(first);
    }
    if(rest)
    {
        analyze_procs(rest);
    }
}

void analyze_proc(Proc proc)
{
	slotNum = 0;
	analyze_head(proc->header);
	if(proc->decls)
		analyze_decls(proc->decls, proc->header->id);
	if(proc->body){
		analyze_stmts(proc->body, proc->header->id);
	}
}

void analyze_head(Header header)
{
	char* procName = header->id;
    if(addProc(procName) == 0){
    	printf("duplicate procedure name \"%s\"\n", procName);
    	errorNum++;
    }
    else if(header->params)
    	analyze_params(header->params, procName,0);
    if(strcmp(procName, "main") == 0)
    	hasMain = 1;
}

void analyze_params(Params params, char* procName, int paramNum) {
	Param first = params->first;
	Params rest = params->rest;
	if (first) {
		analyze_pram(first, procName,paramNum);
		if (rest) {
            analyze_params(rest, procName,paramNum+1);
		}
	}
}
void analyze_pram(Param param, char* procName, int paramNum)
{

	char* varName = param->id;
	if(param->kind==VAL)
	{
		if(addDecl(procName, varName, param->type, slotNum,0, paramNum)==0){
			printf("duplicate parameter of %s in proc %s. \n", varName, procName);
			errorNum++;
		}
		else
			slotNum++;
	}
	else if(param->kind==REF)
	{
		if(addDecl(procName, varName, param->type, slotNum,1,paramNum)==0){
			printf("duplicate parameter of %s in proc %s. \n", varName, procName);
			errorNum++;
		}
		else
			slotNum++;
	}
}

void analyze_decls(Decls decls, char* procName)
{
	Decl first = decls->first;
	Decls rest = decls->rest;
	if(first)
		analyze_decl(first, procName);
	if(rest)
		analyze_decls(rest, procName);
}

void analyze_decl(Decl decl, char* procName)
{
	char* varName = decl->id;
	if(!decl->intervals)
	{
		if(addDecl(procName, varName, decl->type, slotNum,0,-1)==0){
			printf("duplicate declaration of %s in line %d. \n", varName, decl->lineno);
			errorNum++;
		}
		else
			slotNum++;
	}
	else if(decl->intervals)
	{
		int size = getArrSize(decl->intervals);
		int dimension = getArrDimension(decl->intervals);
		if(size == -1)
		{
			printf("lower bound greater than higher bound.\n");
			errorNum++;
		}
		else if(addArray(procName, varName, decl->type, slotNum, size, dimension)==0){
			printf("duplicate declaration of %s in line %d. \n", varName, decl->lineno);
			errorNum++;
		}
		else
			slotNum = slotNum + size;
	}
}

int getArrDimension(Intervals intervals)
{
	int d = 0;
	Intervals i = intervals;
	while(i)
	{
		d++;
		i = i->rest;
	}
	return i;
}

int getArrSize(Intervals intervals)
{
	int s = 0;
	Intervals i = intervals;
	while(i)
	{
		if(i->first->start > i->first->end)
		{
			return -1;
		}
		else
			s = s + i->first->end - i->first->start + 1;
		i = i->rest;
	}
	return s;
}

void analyze_stmts(Stmts stmts, char* procName)
{
	Stmt first = stmts->first;
	Stmts rest = stmts->rest;
	if(first)
		analyze_stmt(first, procName);
	if(rest)
		analyze_stmts(rest, procName);
}

void analyze_stmt(Stmt stmt, char* procName)
{
	switch (stmt->kind) {
		case STMT_ASSIGN:
			analyze_assign(stmt->info.assign, procName);
			break;		
		case STMT_ASSIGN_ARRAY:
			analyze_assign_array(stmt->info.assign, procName);
			break;
		case STMT_COND:
			analyze_cond(stmt->info.cond, procName);
			break;
		case STMT_WHILE:
			analyze_while(stmt->info.loop, procName);
			break;
		case STMT_READ:
			analyze_read(stmt, procName);
			break;		
		case STMT_READ_ARRAY:
			analyze_read_array(stmt, procName);
			break;
		case STMT_WRITE:
			analyze_write(stmt, procName);
			break;
		case STMT_FNCALL:
			analyze_fncall(stmt, procName);
			break;
	}
}

void analyze_assign(Assign assign, char* procName)
{
	analyze_expr(assign.expr, procName);
	int r = checkType(procName, assign.id, getExprType(assign.expr, procName));
	if(getType(procName,assign.id)==FLOAT_TYPE && getExprType(assign.expr, procName)==INT_TYPE)
		r = 1;
	//printf("%d,%d\n",getType( procName,assign.id), getExprType(assign.expr, procName));
	if(r == 0)
	{
		printf("wrong assign type of %s.\n", assign.id);
		errorNum++;
	}
	if(r == -1)
	{
		printf("no declaration of %s.\n", assign.id);
		errorNum++;
	}
}

void analyze_assign_array(Assign assign, char* procName)
{
	//...
}

void analyze_cond(Cond cond_stmt, char* procName)
{
	analyze_expr(cond_stmt.cond, procName);
	if(getExprType(cond_stmt.cond, procName)!=BOOL_TYPE)
	{
		printf("condition expression type is not bool in line %d.\n", cond_stmt.cond->lineno);
		errorNum++;
	}
	analyze_stmts(cond_stmt.then_branch, procName);
	if (cond_stmt.else_branch) {
		analyze_stmts(cond_stmt.else_branch, procName);
	}
}

void analyze_while(While while_stmt, char* procName)
{
	analyze_expr(while_stmt.cond, procName);
	if(getExprType(while_stmt.cond, procName)!=BOOL_TYPE)
	{
		printf("condition expression type is not bool in line %d.\n", while_stmt.cond->lineno);
		errorNum++;
	}
	if(while_stmt.body){
		analyze_stmts(while_stmt.body, procName);
	}
}

void analyze_read(Stmt stmt, char* procName)
{
	if(inDeclared(procName, stmt->info.read) != 1)
	{
		printf("no declaration of %s in line %d.\n", stmt->info.read, stmt->lineno);
		errorNum++;
	}
}

void analyze_read_array(Stmt stmt, char* procName)
{
	//...
}

void analyze_write(Stmt stmt, char* procName)
{
	analyze_expr(stmt->info.write, procName);
}

void analyze_fncall(Stmt stmt, char* procName)
{
	if(procExist(stmt->info.fncall.id)==0)
	{
		printf("function %s dosen't exist.\n", stmt->info.fncall.id);
		errorNum++;
	}
	else if(stmt->info.fncall.args)
	{
		analyze_args(stmt->info.fncall.args, procName, stmt->info.fncall.id);
	}
}

void analyze_args(Exprs args, char* procName, char* callee)
{
	Expr first = args->first;
	Exprs rest = args->rest;
	if(first)
	{
		analyze_expr(first, procName);
		analyze_arg(first, callee);
	}
	else if(rest)
		analyze_args(rest, procName, callee);

}

void analyze_arg(Expr arg, char* callee)
{
	//////////////////////
}

void analyze_expr(Expr expr, char* procName)
{
	switch (expr->kind) {
		case EXPR_ID:
			if(inDeclared(procName, expr->id) != 1)
			{
				printf("no declaration of %s in line %d.\n", expr->id, expr->lineno);
				errorNum++;
			}
			break;
		case EXPR_CONST:
			break;
		case EXPR_BINOP:
			analyze_expr(expr->e1, procName);
			analyze_expr(expr->e2, procName);
			break;
		case EXPR_UNOP:
			analyze_expr(expr->e1, procName);
			break;
		case EXPR_RELOP:
			analyze_expr(expr->e1, procName);
			analyze_expr(expr->e2, procName);
			break;
		case EXPR_ARRAY:
			//...
			break;
	}
}

Type getExprType(Expr expr, char* procName)
{
	Type exprType;
	switch (expr->kind) {
		case EXPR_ID:
			exprType = getType(procName,expr->id);
			break;
		case EXPR_CONST:
			if(expr->constant.type == BOOL_CONSTANT)
				exprType =  BOOL_TYPE;
			else if(expr->constant.type == INT_CONSTANT)
				exprType =  INT_TYPE;
			else if(expr->constant.type == FLOAT_CONSTANT)
				exprType =  FLOAT_TYPE;
			break;
		case EXPR_BINOP:
			if(expr->binop == BINOP_AND || expr->binop ==BINOP_OR)
			{
				if(getExprType(expr->e1, procName)==BOOL_TYPE && getExprType(expr->e2, procName)==BOOL_TYPE)
					exprType = BOOL_TYPE;
				else
					exprType = ERROR_TYPE;
			}
			else if(getExprType(expr->e1, procName)==ERROR_TYPE || getExprType(expr->e2, procName)==ERROR_TYPE)
			{
				exprType =  ERROR_TYPE;
			}
			else if(getExprType(expr->e1, procName)==BOOL_TYPE || getExprType(expr->e2, procName)==BOOL_TYPE)
			{
				printf("wrong binop_expression operand type of bool in line %d.\n", expr->lineno);
				errorNum++;
				exprType =  ERROR_TYPE;
			}
			else if(getExprType(expr->e1, procName)==INT_TYPE && getExprType(expr->e2, procName)==INT_TYPE)
			{
				exprType =  INT_TYPE;
			}
			else
				exprType =  FLOAT_TYPE;
			break;
		case EXPR_UNOP:
			if(expr->binop == UNOP_NOT)
			{
				if(getExprType(expr->e1, procName)==BOOL_TYPE)
					exprType = BOOL_TYPE;
				else
					exprType = ERROR_TYPE;
			}
			else if(getExprType(expr->e1, procName)==BOOL_TYPE)
			{
				printf("wrong Unop_expression type of bool in line %d.\n", expr->lineno);
				errorNum++;
				exprType =  ERROR_TYPE;
			}
			else
				exprType =  getExprType(expr->e1, procName);
			break;
		case EXPR_RELOP:
			if(getExprType(expr->e1, procName)==ERROR_TYPE || getExprType(expr->e2, procName)==ERROR_TYPE)
			{
				exprType =  ERROR_TYPE;
			}
			else if(getExprType(expr->e1, procName)==BOOL_TYPE || getExprType(expr->e2, procName)==BOOL_TYPE)
			{
				printf("wrong relation expression operand type of bool in line %d.\n", expr->lineno);
				errorNum++;
				exprType =  ERROR_TYPE;
			}
			else if(getExprType(expr->e1, procName)!=getExprType(expr->e2, procName))
			{
				printf("relation expression have different operands type in line %d.\n", expr->lineno);
				errorNum++;
				exprType =  ERROR_TYPE;
			}
			else
				exprType =  BOOL_TYPE;
			break;
		case EXPR_ARRAY:
			//...
			break;
		}
		if (exprType == ERROR_TYPE)
		{
			printf("Error type of expression! \n");
			errorNum++;
		}
		return exprType;
}
