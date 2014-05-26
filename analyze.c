/*------------------------------------------------------------------
 Semantic analyzer for the Wiz programs.
 For use in the COMP90045 project 2014.

    Team:       Bandicoot 
    Members:    Wee Lee Tan     377990
                Rui Cheng       664000
                Ryan Leong      395463
 ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "std.h"
#include "symbol.h"
#include "analyze.h"

// Function declarations
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
void analyze_args(Exprs, char*, char*, int);
void analyze_arg(Expr, char*, int, char*);
void analyze_procHeads(Procs);
Type getArrayType(Type);
int getArrDimension(Intervals);
int getArrSize(Intervals);

// use to distribute slot number for each procedure
int slotNum = 0;
// use to identify if there is main function
int hasMain = 0;
// use to count the error number
int errorNum = 0;


/*  
    main function of sementic analyzing
    return the total number of sementic
    error
*/
int analyze(FILE *fp, Program prog)
{
    analyze_procHeads(prog->procs);
    if(hasMain==0)
    {
        printf("No main procedure!\n");
        errorNum++;
    }
    analyze_procs(prog->procs);
    return errorNum;
}
/*
    analyzing heads of all procedures
*/
void analyze_procHeads(Procs procs)
{
Proc first = procs->first;
    Procs rest = procs->rest;
    if(first)
    {
        analyze_head(first);
    }
    if(rest)
    {
        analyze_procHeads(rest);
    }
}
/*
    analyzing the head of each procedure,
    if the procedure name has not been
    defined then put the procedure name 
    into the symbol table, else report an
    error
    also identify if there is an 
    main function in whole program
    also analyze parameters and declarations
*/
void analyze_head(Proc proc)
{
    char* procName = proc->header->id;
    if(addProc(procName) == 0)
    {
        printf("duplicate procedure name \"%s\"\n", procName);
        errorNum++;
    }
    slotNum = 0;
    if(proc->header->params)
        analyze_params(proc->header->params, proc->header->id,0);
    
    if(proc->decls)
        analyze_decls(proc->decls, proc->header->id);
    if(strcmp(proc->header->id, "main") == 0)
        hasMain = 1;
}
/*
    analyzing all procedures
*/
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
/*
    analyzing body of each procedure
*/
void analyze_proc(Proc proc)
{
    if(proc->body)
    {
        analyze_stmts(proc->body, proc->header->id);
    }
}
/*
    analyzing parameters of each procedure
*/
void analyze_params(Params params, char* procName, int paramNum) 
{
    Param first = params->first;
    Params rest = params->rest;
    if (first) 
    {
        analyze_pram(first, procName,paramNum);
        if (rest) 
        {
            analyze_params(rest, procName,paramNum+1);
        }
    }
}
/*
    analyzing each parameter put them into symbol table
    if two parameters have same id then report an error
    0 or 1 indicates val or ref
*/
void analyze_pram(Param param, char* procName, int paramNum)
{
    char* varName = param->id;
    if(param->kind==VAL)
    {
        if(addDecl(procName, varName, param->type, slotNum,0, paramNum)==0)
        {
            printf("duplicate parameter of %s in proc %s. \n", 
                varName, procName);
            errorNum++;
        }
        else
            slotNum++;
    }
    else if(param->kind==REF)
    {
        if(addDecl(procName, varName, param->type, slotNum,1,paramNum)==0)
        {
            printf("duplicate parameter of %s in proc %s. \n", 
                varName, procName);
            errorNum++;
        }
        else
            slotNum++;
    }
}
/*
    analyzing declarations of each procedure
*/
void analyze_decls(Decls decls, char* procName)
{
    Decl first = decls->first;
    Decls rest = decls->rest;
    if(first)
        analyze_decl(first, procName);
    if(rest)
        analyze_decls(rest, procName);
}
/*
    analyzing each declaration, if the variable has not 
    been declared then put it into symbol table,
    else report an error
    for array, calculate the size and dimension
    if the size is -1, report an error
*/
void analyze_decl(Decl decl, char* procName)
{
    char* varName = decl->id;
    if(!decl->intervals)
    {
        if(addDecl(procName, varName, decl->type, slotNum,0,-1)==0)
        {
            printf("duplicate declaration of %s in line %d. \n", 
                                        varName, decl->lineno);
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
        else if(addArray(procName, varName, decl->type, slotNum,
                           size, dimension, decl->intervals)==0)
        {
            printf("duplicate declaration of %s in line %d. \n",
                                          varName, decl->lineno);
            errorNum++;
        }
        else
            slotNum = slotNum + size;
    }
}
/*
    calculate and return the dimension of an array
*/
int getArrDimension(Intervals intervals)
{
    int d = 0;
    Intervals i = intervals;
    while(i)
    {
        d++;
        i = i->rest;
    }
    return d;
}
/*
    calculate and return the size of an array,
    if the lower bound is greater than upper bound
    in any dimension then return -1
*/
int getArrSize(Intervals intervals)
{
    int s = 1;
    Intervals i = intervals;
    
    while(i)
    {
        if(i->first->start > i->first->end)
        {
            return -1;
        }
        else{
            int z = i->first->end - i->first->start + 1;
            s *= z;
        }
        i = i->rest;
    }
    return s;
}

/*
    analyzing all statements of each procedure
*/
void analyze_stmts(Stmts stmts, char* procName)
{
    Stmt first = stmts->first;
    Stmts rest = stmts->rest;
    if(first)
        analyze_stmt(first, procName);
    if(rest)
        analyze_stmts(rest, procName);
}
/*
    analyzing each statement of procedure
*/
void analyze_stmt(Stmt stmt, char* procName)
{
    switch (stmt->kind) {
        case STMT_ASSIGN:
            analyze_assign(stmt->info.assign, procName);
            break;        
        case STMT_ASSIGN_ARRAY:
            analyze_assignArray(stmt->info.assign, procName);
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
            analyze_read(stmt, procName);
            break;
        case STMT_WRITE:
            analyze_write(stmt, procName);
            break;
        case STMT_FNCALL:
            analyze_fncall(stmt, procName);
            break;
    }
}
/*
    analyzing assignment statement,
    if any variable is not declared report an error
    check the type of left and right
*/
void analyze_assign(Assign assign, char* procName)
{
    analyze_expr(assign.expr, procName);
    int r = checkType(procName, assign.id, 
        getExprType(assign.expr, procName));

    int j = getArrayType(getType(procName,assign.id)) == 
        getExprType(assign.expr, procName) ? 1:0;

    if(getType(procName,assign.id)==FLOAT_TYPE && 
        getExprType(assign.expr, procName)==INT_TYPE ||
         getType(procName,assign.id)==FLOAT_TYPE && 
          getExprType(assign.expr, procName)==INT_ARRAY_TYPE)
        r = 1;
    if(r == 0 && j == 0)
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
/*
    change the expression type to orresponding
    array type
*/
Type getArrayType(Type type)
{
    switch(type)
    {
        case INT_TYPE:
        case INT_ARRAY_TYPE:
            return INT_ARRAY_TYPE;
        case FLOAT_TYPE:
        case FLOAT_ARRAY_TYPE:
            return FLOAT_ARRAY_TYPE;
        case BOOL_TYPE:
        case BOOL_ARRAY_TYPE:
            return BOOL_ARRAY_TYPE;
        default:
            return ERROR_TYPE;
    }
}
/*
    analyzing assignment statement of array,
    if any variable is not declared report an error
    check the type of left and right
*/
void analyze_assignArray(Assign assign, char* procName)
{
    analyze_expr(assign.expr, procName);
    int r = checkType(procName, assign.id, 
             getArrayType(getExprType(assign.expr, procName)));
    if(getType(procName,assign.id)==FLOAT_ARRAY_TYPE &&
         getArrayType(getExprType(assign.expr, procName))==INT_ARRAY_TYPE)
        r = 1;
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
/*
    analyzing condition statements of procedure
    check if the condition expression is bool type
*/
void analyze_cond(Cond cond_stmt, char* procName)
{
    analyze_expr(cond_stmt.cond, procName);
    if(getExprType(cond_stmt.cond, procName)!=BOOL_TYPE)
    {
        printf("condition expression type is not ");
        printf("bool in line %d.\n", cond_stmt.cond->lineno);
        errorNum++;
    }
    analyze_stmts(cond_stmt.then_branch, procName);
    if (cond_stmt.else_branch) 
    {
        analyze_stmts(cond_stmt.else_branch, procName);
    }
}
/*
    analyzing while statements of procedure
    check if the condition expression is bool type
*/
void analyze_while(While while_stmt, char* procName)
{
    analyze_expr(while_stmt.cond, procName);
    if(getExprType(while_stmt.cond, procName)!=BOOL_TYPE)
    {
        printf("condition expression type is not bool in");
        printf(" line %d.\n", while_stmt.cond->lineno);
        errorNum++;
    }
    if(while_stmt.body){
        analyze_stmts(while_stmt.body, procName);
    }
}
/*
    analyzing read statements of procedure
    check if the variable is declared
*/
void analyze_read(Stmt stmt, char* procName)
{
    if(inDeclared(procName, stmt->info.read) != 1)
    {
        printf("no declaration of %s in line %d.\n",
              stmt->info.read, stmt->lineno);
        errorNum++;
    }
}
/*
    analyzing write statements of procedure
*/
void analyze_write(Stmt stmt, char* procName)
{
    analyze_expr(stmt->info.write, procName);
}
/*
    analyzing function call statements of procedure
    check if the callee function is defined
*/
void analyze_fncall(Stmt stmt, char* procName)
{
    if(procExist(stmt->info.fncall.id)==0)
    {
        printf("function %s dosen't exist.\n", stmt->info.fncall.id);
        errorNum++;
    }
    else if(stmt->info.fncall.args)
    {
        analyze_args(stmt->info.fncall.args,procName,stmt->info.fncall.id,0);
    }
}
/*
    analyzing all arguments of the function call
*/
void analyze_args(Exprs args, char* procName, char* callee, int paramNum)
{
    Expr first = args->first;
    Exprs rest = args->rest;
    if(first && getParamType(callee, paramNum)!=-1)
    {
        analyze_expr(first, procName);
        analyze_arg(first, callee, paramNum, procName);
    }
    else if(first && getParamType(callee, paramNum)==-1)
    {
        printf("function call %s has more number of parameter.\n",
                                                    callee);
        errorNum++;
        return;
    }
    if(rest == 0 && getParamType(callee, paramNum+1)!=-1)
    {
        printf("function call %s has less number of parameter.\n",
                                                    callee);
        errorNum++;
    }
    if(rest)
        analyze_args(rest, procName, callee, paramNum+1);

}
/*
    analyzing each argument of the function call
    check the arguments number and type with the
    defination of the callee
*/
void analyze_arg(Expr arg, char* callee, int paramNum, char* procName)
{
    if(isParamRef(callee, paramNum)==1)
    {
        // ref parameters
        if((getParamType(callee, paramNum) != getExprType(arg, procName)) &&
            (getArrayType(getParamType(callee, paramNum)) != 
                              getExprType(arg, procName)))
        {
            printf("No.%d parameter of function call %s has wrong type.\n",
                paramNum+1, callee);
            errorNum++;
        }
    }
    else if(isParamRef(callee, paramNum)==0)
    {
        // val parameters
        if((getParamType(callee, paramNum) != getExprType(arg, procName)) &&
            (getArrayType(getParamType(callee, paramNum)) != 
                              getExprType(arg, procName)))
        {
            if(getParamType(callee, paramNum) == FLOAT_TYPE && 
                (getExprType(arg, procName) == INT_TYPE ||
                  getExprType(arg, procName) == INT_ARRAY_TYPE))
            {}
            else
            {
                printf("No.%d parameter of function call %s has wrong type.\n",
                    paramNum+1, callee);
                errorNum++;
            }
        }
    }
}
/*
    analyzing expressions of each statement
*/
void analyze_expr(Expr expr, char* procName)
{
    switch (expr->kind) {
        case EXPR_ID:
            if(inDeclared(procName, expr->id) != 1)
            {
                printf("no declaration of %s in line %d.\n", 
                                   expr->id, expr->lineno);
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
            if(inDeclared(procName, expr->id) != 1)
            {
                printf("no declaration of %s in line %d.\n", 
                                   expr->id, expr->lineno);
                errorNum++;
            }
            analyze_exprs(expr->es,procName);
            break;
    }
}
/*
    analyzing expression list for array
*/
void analyze_exprs(Exprs exprs, char* procName)
{
    Expr first = exprs->first;
    Exprs rest = exprs->rest;
    if(first)
        analyze_expr(first, procName);
    if(rest)
        analyze_exprs(rest, procName);
}
/*
    analyzing the type of an expression
    return the type of the expression,
    if there is an type error return ERROR_TYPE
    and report the error
*/
Type getExprType(Expr expr, char* procName)
{
    Type exprType;
    switch (expr->kind)
    {
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
            if((expr->binop == BINOP_AND) || (expr->binop ==BINOP_OR))
            {
                // 'and' and 'or' must have bool type operands
                if((getExprType(expr->e1, procName)==BOOL_TYPE||
                     getExprType(expr->e1, procName)==BOOL_ARRAY_TYPE) && 
                      (getExprType(expr->e2, procName)==BOOL_TYPE||
                        getExprType(expr->e2, procName)==BOOL_ARRAY_TYPE))
                    exprType = BOOL_TYPE;
                else
                    exprType = ERROR_TYPE;
            }
            else if(getExprType(expr->e1, procName)==ERROR_TYPE || 
                     getExprType(expr->e2, procName)==ERROR_TYPE)
            {
                exprType =  ERROR_TYPE;
            }
            else if((getExprType(expr->e1, procName)==BOOL_TYPE||
                     getExprType(expr->e1, procName)==BOOL_ARRAY_TYPE) && 
                      (getExprType(expr->e2, procName)==BOOL_TYPE||
                        getExprType(expr->e2, procName)==BOOL_ARRAY_TYPE))
            {
                // other binary operators must not have bool type operands
                printf("wrong binop_expression operand ");
                printf("type of bool in line %d.\n", expr->lineno);
                errorNum++;
                exprType =  ERROR_TYPE;
            }
            else if((getExprType(expr->e1, procName)==INT_TYPE||
                      getExprType(expr->e1, procName)==INT_ARRAY_TYPE) && 
                       (getExprType(expr->e2, procName)==INT_TYPE||
                         getExprType(expr->e2, procName)==INT_ARRAY_TYPE))
            {
                // if both operands are 'int' type, 
                // the expression is 'int' type
                exprType =  INT_TYPE;
            }
            else
                // if one of the operands is 'float' type,
                // the expression is 'float' type
                exprType =  FLOAT_TYPE;
            break;
        case EXPR_UNOP:
            if(expr->unop == UNOP_NOT)
            {
                // 'not' operator must have bool type operand
                if(getExprType(expr->e1, procName)==BOOL_TYPE||
                    getExprType(expr->e1, procName)==BOOL_ARRAY_TYPE)
                    exprType = BOOL_TYPE;
                else
                    exprType = ERROR_TYPE;
            }
            else if(getExprType(expr->e1, procName)==BOOL_TYPE||
                     getExprType(expr->e1, procName)==BOOL_ARRAY_TYPE)
            {
                //'-' operator must not have bool type operand
                printf("wrong Unop_expression type of bool in line %d.\n"
                    , expr->lineno);
                errorNum++;
                exprType =  ERROR_TYPE;
            }
            else
                exprType =  getExprType(expr->e1, procName);
            break;
        case EXPR_RELOP:
            
            if(getExprType(expr->e1, procName)==ERROR_TYPE || 
                getExprType(expr->e2, procName)==ERROR_TYPE)
            {
                exprType =  ERROR_TYPE;
            }
            else if (expr->relop == RELOP_EQ || expr->relop == RELOP_NE)
            {
                if(getExprType(expr->e1, procName) != 
                    getExprType(expr->e2, procName) &&
                     getExprType(expr->e1, procName) != 
                      getArrayType(getExprType(expr->e2, procName))&&
                       getArrayType(getExprType(expr->e1, procName))!=
                        getExprType(expr->e2, procName))
                    printf("relation expression have different operands type.\n");
                    errorNum++;
                    exprType =  ERROR_TYPE;
            }
            else if((getExprType(expr->e1, procName)==BOOL_TYPE||
                     getExprType(expr->e1, procName)==BOOL_ARRAY_TYPE)&& 
                      (getExprType(expr->e2, procName)==BOOL_TYPE||
                        getExprType(expr->e2, procName)==BOOL_ARRAY_TYPE))
            {
                // relation operator must not have bool type operand
                printf("wrong relation expression operand type of bool.\n");
                errorNum++;
                exprType =  ERROR_TYPE;
            }
            else
                // relation expression is bool type
                exprType =  BOOL_TYPE;
            break;
            case EXPR_ARRAY:
                exprType = getType(procName, expr->id);
            break;
        }
        if (exprType == ERROR_TYPE)
        {
            // report an error
            printf("Error type of expression!\n");
            errorNum++;
        }
        return exprType;
}
