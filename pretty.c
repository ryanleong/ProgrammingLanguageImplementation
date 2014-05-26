/*-----------------------------------------------------------------------
    A pretty-printer for Wiz programs.
    For use in the COMP90045 project 2014.

    Team:       Bandicoot 
    Members:    Wee Lee Tan     377990
                Rui Cheng       664000
                Ryan Leong      395463
-----------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "std.h"
#include "codegen.h"
#include "symbol.h"
#include "analyze.h"

int loopcount = 0;
int ifcount = 0;

void pretty_prog(FILE *fp, Program prog) {
	Procs sorted_procs = sort_procs(prog->procs);
	printf("call proc_main\n");
	printf("halt\n");
	printf("\n");
	print_procs(sorted_procs);
}

static void print_procs(Procs procs) {
	Proc first = procs->first;
	Procs rest = procs->rest;
	if (first) {
		print_proc(first);
		printf("\n");

	}
	if (rest) {
		print_procs(rest);
	}
}

static void print_proc(Proc proc) {
	printf("proc_%s:", proc->header->id);
	int stack_count = getStackSize(proc->header->id);
	printf("\n");
	printf("#prologue\n");
	if (proc->decls||proc->header->params) {
		//Get the number of declarations.
		printf("push_stack_frame %d", stack_count);
		printf("\n");
		if(proc->header->params)
			print_params(proc->header->params,proc->header->id);
		printf("int_const r0, 0\n");
		printf("real_const r1, 0.0\n");
		printf("\n");
		
		if(proc->decls)
			print_decls(proc->decls, 1, proc->header->id);
	}
	printf("\n");
	if (proc->body) {
		print_stmts(proc->body, 1, proc->header->id);
	}
	printf("#epilogue\n");
	if (stack_count != 0) 
		printf("pop_stack_frame %d\n", stack_count);
	printf("return\n");
}


static void print_decls(Decls decls, int l, char* proc_id) {
	Decl first = decls->first;
	Decls rest = decls->rest;
	if (first) {
		print_decl(first, l, proc_id);
	}
	if (rest) {
		print_decls(rest, l, proc_id);
	}
}




static void print_params(Params params, char* procName) {
	Params p = params;
	int slot;
	int i = 0;
	while (p)
	{
		slot = getStackSlotNum(procName, p->first->id);
		if (p->first->kind==VAL)
		{
			printf("store %d, r%d\n", slot, i);
		}
		else if(p->first->kind==REF)
		{
			printf("store %d, r%d\n", slot, i);
		}
		p = p->rest;
		i++;
	}
}

//Printing declarations codes
static void print_decl(Decl decl, int l, char* proc_id) {
	int slot;
	slot = getStackSlotNum(proc_id, decl->id);
	switch (decl->type) {
		case BOOL_TYPE:
			
			printf("store %d, r0", slot);
			printf("\n");
			break;
		case INT_TYPE:
			
			printf("store %d, r0", slot);
			printf("\n");
			break;
		case FLOAT_TYPE:
			
			printf("store %d, r1", slot);
			printf("\n");
			break;
		case INT_ARRAY_TYPE:{
			int i;
			for (i = 0; i < getArraySize(proc_id, decl->id); i++){
				printf("store %d, r0", slot);
				printf("\n");
				slot++;
			}
			break;
			}
		case FLOAT_ARRAY_TYPE:{
			int i;
			for (i = 0; i < getArraySize(proc_id, decl->id); i++){
				printf("store %d, r1", slot);
				printf("\n");
				slot++;
			}	
			break;	
			}
		case BOOL_ARRAY_TYPE:{
			int i;
			for (i = 0; i < getArraySize(proc_id, decl->id); i++){
				printf("store %d, r0", slot);
				printf("\n");
				slot++;
			}
			break;
			}
	}
}


//Remove if unused
static void print_intervals(Intervals intervals) {
	Interval first = intervals->first;
	Intervals rest = intervals->rest;
	if (first) {
		print_interval(first);
		if (rest) {
			printf(",");
			print_intervals(rest);
		}
	}
}

static void print_interval(Interval interval) {
	printf("%d..%d", interval->start, interval->end);
}

static void print_stmts(Stmts stmts, int l, char* proc_id) {
	Stmt first = stmts->first;
	Stmts rest = stmts->rest;
	if (first) {
		print_stmt(first, l, proc_id);
	}
	if (rest) {
		print_stmts(rest, l, proc_id);
	}
}

//Print conditional statements
static Type print_cond(Expr expr, char* proc_id, int reg, 
			int stmt_type, char* label1, char* label2) {

	// Print label if while loop
	if (stmt_type == 1) {
		// Add label to branch to at top of loop
		printf("%s:\n", label1);
	}

	switch(expr->kind) {
		case EXPR_ID: {
			// Get type of expression
			Type exprType = getType(proc_id,expr->id);
			printf("load r%d, %d\n", reg, exprType);

			if (stmt_type == 0) {
				// print branch for ifcond
				printf("branch_on_false r%d, %s\n", reg, label1);
			}
			else if (stmt_type == 1) {
				// print branch for while
				printf("branch_on_false r%d, %s\n", reg, label2);
			}

			return exprType;
			break;
		}
		case EXPR_CONST:

			// print int constant
			printf("int_const r%d, %d\n", reg, expr->constant.val.bool_val);

			if (stmt_type == 0) {
				// print branch for ifcond
				printf("branch_on_false r%d, %s\n", reg, label1);
			}
			else if (stmt_type == 1) {
				// print branch for while
				printf("branch_on_false r%d, %s\n", reg, label2);
			}

			switch(expr->constant.type){
				case INT_CONSTANT:
					return INT_TYPE;
					break;
				case FLOAT_CONSTANT:
					return FLOAT_TYPE;
					break;
				case BOOL_CONSTANT:
					return BOOL_TYPE;
					break;
			}

			break;
		case EXPR_BINOP: {

			// print binary operations
			reg = print_expr(expr->e1, reg, proc_id);
			int reg1 = print_expr(expr->e2, reg+1, proc_id);
			int ID_type = getExprType(expr->e1, proc_id);
			int ID_type2 = getExprType(expr->e2, proc_id);
			print_binop_string(expr->binop, reg, reg1, ID_type, ID_type2);

			if (stmt_type == 0) {
				// print branch for ifcond
				printf("branch_on_false r%d, %s\n", reg, label1);
			}
			else if (stmt_type == 1) {
				// print branch for while
				printf("branch_on_false r%d, %s\n", reg, label2);
			}

			break;
		}
		case EXPR_RELOP: {

			// print expressions in if or while condition
			int reg1 = print_expr(expr->e1, reg, proc_id);
			int reg2 = print_expr(expr->e2, reg+1, proc_id);

			Type e1Type = getExprType(expr->e1, proc_id);
			Type e2Type = getExprType(expr->e2, proc_id);

			Type evalType = -1;

			// Compare types to see which to use
			if (e1Type == INT_TYPE && e2Type == INT_TYPE) {
				evalType = INT_TYPE;
			}
			else if ((e1Type == FLOAT_TYPE && e2Type == FLOAT_TYPE) ||
				(e1Type == INT_TYPE && e2Type == FLOAT_TYPE) || 
				(e1Type == FLOAT_TYPE && e2Type == INT_TYPE)) {

				evalType = FLOAT_TYPE;
			}
			else if (e1Type == BOOL_TYPE && e2Type == BOOL_TYPE) {
				evalType = BOOL_TYPE;
			}

			// print relational operation
			print_relop_string(expr->relop, reg1, reg2, e1Type, e2Type);

			if (stmt_type == 0) {
				// print branch for ifcond
				printf("branch_on_false r%d, %s\n", reg1, label1);
			}
			else if (stmt_type == 1) {
				// print branch for while
				printf("branch_on_false r%d, %s\n", reg1, label2);
			}

			break; 
		}
	}
	printf("\n");
	return -1;
}

static print_assign_array(Assign assign, Exprs expr, char* proc_id) {

	// calculate offset
	int reg = calculate_offset(expr, 0, assign.id, proc_id);

	int next_reg = print_expr(expr, 1, proc_id);

	printf("store_indirect r%d, r%d\n", reg, next_reg);
}

//Printing each statement
static void print_stmt(Stmt stmt, int l, char* proc_id) {
	switch (stmt->kind) {
		case STMT_ASSIGN:
			print_assign(stmt->info.assign, l, proc_id);
			break;		
		case STMT_ASSIGN_ARRAY:
			print_assign_array(stmt->info.assign, stmt->info.assign.exprs, proc_id);
			break;
		case STMT_COND:{
			printf("#if\n");

			// Form label
			char ifCountStr[15];
			char str[80];
			if (stmt->info.cond.else_branch != NULL) {
				sprintf(ifCountStr, "%d", ifcount);
				strcpy (str,"else_");
				strcat (str,ifCountStr);
			}
			else {
				sprintf(ifCountStr, "%d", ifcount);
				strcpy (str,"end_if_");
				strcat (str,ifCountStr);
			}

			int currentIfCount = ifcount;
			ifcount++;
			
			//print_cond(stmt->info.cond, l);
			print_cond(stmt->info.cond.cond, proc_id, 0, 0, str, "");

			// Print then statements
			print_stmts(stmt->info.cond.then_branch, l, proc_id);


			char endIfLabel[80];
			char currCountStr[15];
			sprintf(currCountStr, "%d", currentIfCount);
			strcpy (endIfLabel,"end_if_");
			strcat (endIfLabel,currCountStr);

			// Print else statements
			if (stmt->info.cond.else_branch != NULL) {
				// Branch unconditional
				printf("branch_uncond %s\n", endIfLabel);

				// Do False
				printf("%s:\n", str);

				print_stmts(stmt->info.cond.else_branch, l, proc_id);
			}
		
			// print end label
			printf("%s:\n", endIfLabel);

			printf("\n");
			break;
		}
		case STMT_WHILE:{
			printf("#while\n");

			// print while labels
			char whileStr[15];
			sprintf(whileStr, "%d", loopcount);
			char top[80];
			strcpy (top,"top_of_loop_");
			strcat (top,whileStr);

			char loopbreak[80];
			strcpy (loopbreak,"loop_is_false_");
			strcat (loopbreak,whileStr);

			int currentLoopCount = loopcount;
			loopcount++;

			// print while condition
			print_cond(stmt->info.loop.cond, proc_id, 0, 1, top, loopbreak);

			// Print statements
			print_stmts(stmt->info.loop.body, l, proc_id);

			// Print unconditional branch
			printf("branch_uncond %s\n", top);

			// Print end loop label
			printf("%s:\n", loopbreak);
			printf("\n");

			break;
		}
		case STMT_READ:
			print_read(stmt, l, proc_id);
			break;		
		case STMT_READ_ARRAY: {
			// print_read_array(stmt, l);

			// calculate offset
			int reg = calculate_offset(stmt->info.array.exprs, 1, stmt->info.assign.id, proc_id);

			switch(getType(proc_id, stmt->info.assign.id)) {
				case(INT_ARRAY_TYPE):
					printf("call_builtin read_int \n");
					break;

				case(FLOAT_ARRAY_TYPE):
					printf("call_builtin read_float \n");
					break;

				case(BOOL_ARRAY_TYPE):
					printf("call_builtin read_bool \n");
					break;
			}

			// Store indirect for array
			printf("store_indirect r%d, r%d\n", reg, 0);
			break;
		}
		case STMT_WRITE:
			// Print write statement
			print_write(stmt, l, proc_id);
			break;
		case STMT_FNCALL:
			// Print proc calls
			print_fncall(stmt, l, proc_id);
			break;
	}
}

//Assignment code
static void print_assign(Assign assign, int l, char* proc_id) {

	int ID_type;
	int expr_type;
	int slot;
	ID_type = getType(proc_id,assign.id);
	expr_type = getExprType(assign.expr, proc_id);
	if (isRef(proc_id, assign.id)==0){
		if (ID_type == 2 && expr_type == 1){
			slot = getStackSlotNum(proc_id, assign.id);
			printf("#assignment\n");
			print_expr(assign.expr, 0, proc_id);
			printf("int_to_real r0, r0\n");
			printf("store %d, r0\n", slot);
			printf("\n");
		}
		
		else if ((ID_type == 1 && expr_type == 1) ||
			(ID_type == 2 && expr_type == 2) ||
			(ID_type == 0 && expr_type == 0)){
			slot = getStackSlotNum(proc_id, assign.id);	
			printf("#assignment\n");
			print_expr(assign.expr, 0, proc_id);
			printf("store %d, r0\n", slot);
			printf("\n");
		}
    }
	//For reference parameters
    else if(isRef(proc_id, assign.id)==1){
    	if (ID_type == 2 && expr_type == 1){
			slot = getStackSlotNum(proc_id, assign.id);
			printf("#assignment\n");
			print_expr(assign.expr, 0, proc_id);
			printf("int_to_real r0, r0\n");
			printf("load r1, %d\n", slot);
			printf("store_indirect r1, r0\n");
			printf("\n");
		}
		
		else if ((ID_type == 1 && expr_type == 1) ||
			(ID_type == 2 && expr_type == 2) ||
			(ID_type == 0 && expr_type == 0)){
			slot = getStackSlotNum(proc_id, assign.id);	
			printf("#assignment\n");
			print_expr(assign.expr, 0, proc_id);
			printf("load r1, %d\n", slot);
			printf("store_indirect r1, r0\n");
			printf("\n");
		}
    }
	
}

//Read function call
static void print_read(Stmt stmt, int l, char* proc_id) {
	
	printf("#read");
	printf("\n");
	print_indent(l);
	Type ID_type;
	int stackNo;
	ID_type = getType(proc_id,stmt->info.read);
	stackNo = getStackSlotNum(proc_id, stmt->info.read);
	switch (ID_type){					
		case BOOL_TYPE:
			printf("call_builtin read_bool"); //with a function
			printf("\n");
			print_indent(l);
			printf("store %d, r0", stackNo);
			printf("\n");
			break;
		case INT_TYPE:
			printf("call_builtin read_int \n"); //with a function
			print_indent(l);
			printf("store %d, r0 \n", stackNo);
			break;
		case FLOAT_TYPE:
			printf("call_builtin read_real \n"); //with a function
			print_indent(l);
			printf("store %d, r0 \n", stackNo);
			break; 
	}
	printf("\n");
}

//Remove if unused
static void print_read_array(Stmt stmt, int l) {
	print_indent(l);
	printf("read %s", stmt->info.array.id);
	printf("[");
	printf("];\n");
}

//Write statments
static void print_write(Stmt stmt, int l, char* proc_id) {

	printf("# write\n");
	print_write_expr(stmt->info.write, 1, proc_id);
	printf("\n");
}

//Remove the depth later
static void print_write_expr(Expr expr, int depth, char* proc_id) {
	switch (expr->kind) {
		int ID_type = 0;
		int ID_type2 = 0;
		int stackNo;
		int reg;
		int reg1;
		case EXPR_ID:
			ID_type = getType(proc_id,expr->id);
			stackNo = getStackSlotNum(proc_id, expr->id);
			printf("load r0, %d\n", stackNo);
			switch (ID_type){					
				case BOOL_TYPE:
				printf("\n");
				printf("call_builtin print_bool"); //with a function
				printf("\n");
				break;
				case INT_TYPE:
				//printf("int %s;\n", decl->id);
				printf("call_builtin print_int \n"); //with a function
				break;
				case FLOAT_TYPE:
				//printf("float %s;\n", decl->id);
				printf("call_builtin print_real \n"); //with a function
				break; 
				}
			break;
		case EXPR_CONST:
			print_write_constant(expr->constant);
			break;
		case EXPR_BINOP:
			reg = print_expr(expr->e1, 0, proc_id);
			reg1 = print_expr(expr->e2, 1, proc_id);
			ID_type = getExprType(expr->e1, proc_id);
			ID_type2 = getExprType(expr->e2, proc_id);
			print_binop_string(expr->binop, reg, reg1, ID_type, ID_type2);
			if ((ID_type + ID_type2) >= 3){
				printf("call_builtin print_real \n");
			}
			else if ((ID_type + ID_type2) == 2){
				printf("call_builtin print_int \n");
			}
			else{
				printf("call_builtin print_bool \n");
			}
			break;	
		case EXPR_RELOP:
			reg =  print_expr(expr->e1, 0, proc_id);
			reg1 = print_expr(expr->e2, 1, proc_id);
			ID_type = getExprType(expr->e1, proc_id);
			ID_type2 = getExprType(expr->e2, proc_id);
			print_relop_string(expr->relop, reg, reg1, ID_type, ID_type2);
			printf("call_builtin print_bool \n");
			break;
		case EXPR_UNOP:
			reg =  print_expr(expr->e1, 0, proc_id);
			ID_type = getExprType(expr->e1, proc_id);
			print_unop_string(expr->unop, reg, ID_type);
			if (ID_type == 2){
				printf("call_builtin print_real \n");
			}
			else if (ID_type == 1 ){
				printf("call_builtin print_int \n");
			}
			else{
				printf("call_builtin print_bool \n");
			}
			break;
		case EXPR_ARRAY:
			calculate_offset(expr->es, 0, expr->id, proc_id);
			break;		
	}
}

//Binary operators
void print_binop_string(int binop, int curr_reg, int next_reg, int ID1, int ID2){
	int ID = ID1 + ID2; //To see if it's float or int. float = 3/4.
	if (ID1 == 1 && ID2 == 2){
		printf("int_to_real r%d, r%d\n", curr_reg, curr_reg);
		}
	else if (ID1 == 2 && ID2 == 1){
		printf("int_to_real r%d, r%d\n", next_reg, next_reg);
		}	
	switch(binop){
		case BINOP_ADD:
			if (ID >= 3){
				printf("add_real r%d, r%d, r%d \n", curr_reg, curr_reg, next_reg);
			}
			else{
				printf("add_int r%d, r%d, r%d \n", curr_reg, curr_reg, next_reg);
			}
			break;
		case BINOP_SUB:
			if (ID >= 3){
				printf("sub_real r%d, r%d, r%d \n", curr_reg, curr_reg, next_reg);
				}
			else{
				printf("sub_int r%d, r%d, r%d \n", curr_reg, curr_reg, next_reg);
			}
			break;
		case BINOP_MUL:
			if (ID >= 3){
				printf("mul_real r%d, r%d, r%d \n", curr_reg, curr_reg, next_reg);
				}
			else{
				printf("mul_int r%d, r%d, r%d \n", curr_reg, curr_reg, next_reg);
				}
			break;
		case BINOP_DIV:
			if (ID >= 3){
				printf("div_real r%d, r%d, r%d \n", curr_reg, curr_reg, next_reg);
				}
			else{
				printf("div_int r%d, r%d, r%d \n", curr_reg, curr_reg, next_reg);
				}
			break;
		case BINOP_AND:
			printf("and r%d, r%d, r%d \n", curr_reg, curr_reg, next_reg);
			break;
		case BINOP_OR:
			printf("or r%d, r%d, r%d \n", curr_reg, curr_reg, next_reg);
			break;		
	}
}

//Writing a constant value
static void print_write_constant(Constant constant) {
	switch (constant.type) {
		case BOOL_CONSTANT:
			printf("int_const r0, %d", constant.val.bool_val);
			printf("\n");
			printf("call_builtin print_bool"); //with a function
			printf("\n");
			break;
		case INT_CONSTANT:
			printf("int_const r0, %d", constant.val.int_val);
			printf("\n");
			printf("call_builtin print_int"); //with a function
			printf("\n");
			break;
		case FLOAT_CONSTANT:
			printf("real_const r0, %s", constant.val.float_val);
			printf("\n");
			printf("call_builtin print_real"); //with a function
			printf("\n");
			break;
		case STRING_CONSTANT:
			printf("string_const r0, %s", constant.val.str_val);
			printf("\n");
			printf("call_builtin print_string"); //with a function
			printf("\n");
			break;
	}
}

//Printing expression types
int print_expr(Expr expr, int reg, char* proc_id) {
	int curr_reg = reg;
	int next_reg;
	int ID_type;
	int ID_type2;
	switch (expr->kind) {
		Type ID_type;
		int stackNo;
		case EXPR_ID:
			ID_type = getType(proc_id,expr->id);
			stackNo = getStackSlotNum(proc_id, expr->id);
			printf("load r%d, %d\n", curr_reg,stackNo);
			break;
		case EXPR_CONST:
			print_constant(expr->constant, curr_reg);
			break;
		case EXPR_BINOP:
			ID_type = getExprType(expr->e1, proc_id);
			ID_type2 = getExprType(expr->e2, proc_id);
			curr_reg = print_expr(expr->e1, curr_reg, proc_id);
			next_reg = print_expr(expr->e2, curr_reg + 1, proc_id);
			print_binop_string(expr->binop, curr_reg, next_reg, ID_type, ID_type2);
			break;
		case EXPR_RELOP:
			ID_type = getExprType(expr->e1, proc_id);
			ID_type2 = getExprType(expr->e2, proc_id);
			
			curr_reg = print_expr(expr->e1, curr_reg, proc_id);
			next_reg = print_expr(expr->e2, curr_reg + 1, proc_id);
			print_relop_string(expr->relop, curr_reg, next_reg, ID_type, ID_type2);
			break;
		case EXPR_UNOP:	
			ID_type = getExprType(expr->e1, proc_id);
			print_unop_string(expr->unop, curr_reg, ID_type);
			break;
		case EXPR_ARRAY:
			curr_reg = calculate_offset(expr->es, curr_reg, expr->id, proc_id);
			break;	
			}
	return curr_reg;		
}

int calc(int highBounds[], int lowBounds[], int currDimension, int dimension, int reg, int nextFreeReg) {

	if (currDimension <= dimension) {

		int temp = (highBounds[currDimension-1] - lowBounds[currDimension-1]) + 1;

		printf("int_const r%d, %d\n", nextFreeReg, temp);

		int next_reg = calc(highBounds, lowBounds, currDimension+1, dimension, reg+1, nextFreeReg+1);

		if(currDimension != dimension)
			printf("mul_int r%d, r%d, r%d\n", nextFreeReg, nextFreeReg, nextFreeReg+1);	

		return nextFreeReg;
	}
	else {
		return reg-1;
	}
}

int calc2(int highBounds[], int lowBounds[], int currentDimension, int dimension, int next_reg, int curr_reg) {
	
	if (currentDimension ==  dimension) {
		printf("int_const r%d, %d\n", next_reg, lowBounds[currentDimension-1]);
		printf("sub_int r%d, r%d, r%d\n", next_reg, curr_reg, next_reg);
		printf("add_int r%d, r%d, r%d\n", next_reg-1, next_reg-1, next_reg);
		return -1;
	}

	printf("int_const r%d, %d\n", next_reg, lowBounds[currentDimension-1]);
	printf("sub_int r%d, r%d, r%d\n", next_reg, curr_reg, next_reg);
	printf("\n");
	// calculate for each dimension
	int stored_reg = calc(highBounds, lowBounds, currentDimension+1, dimension, curr_reg, next_reg+1);


	printf("\n");
	printf("mul_int r%d, r%d, r%d\n", next_reg, next_reg, stored_reg);
	printf("\n");

	calc2(highBounds, lowBounds, currentDimension+1, dimension, next_reg+1, curr_reg+1);

	if(currentDimension != 1)
		printf("add_int r%d, r%d, r%d\n", next_reg-1, next_reg-1, next_reg);

	return 0;
}

int calculate_offset(Exprs expr, int reg, char* id, char* proc_id){
	int stackNo;
	int curr_reg = reg;
	
	int dimension;
	int size;
	Intervals array_interval;
	stackNo = getStackSlotNum(proc_id, id);
	print_exprs(expr, curr_reg, proc_id);
	dimension = getArrayDimension(proc_id, id);
	size = getArraySize(proc_id, id);
	int bounds[size];
	array_interval = getIntervals(proc_id, id);
	int j = 0;
	int next_reg = curr_reg+dimension;


	int highBounds[dimension];
	int lowBounds[dimension];

	while (array_interval != NULL){
		highBounds[j] = array_interval->first->end;
		lowBounds[j] = array_interval->first->start;

		j++;
		array_interval = array_interval->rest;
	}

	int currentDimension = 1;

	calc2(highBounds, lowBounds, currentDimension, dimension, next_reg, curr_reg);
	
	printf("load_address r%d, %d\n", 0, stackNo);
	printf("sub_offset r%d, r%d, r%d\n", curr_reg, curr_reg, next_reg);

	// store_indirect r1, r0 # 

	return curr_reg;
}

//Function parameters
int print_arg(Expr expr, int reg, char* proc_id,int paramNum,char* callee) {
	int curr_reg = reg;
	int next_reg;
	int ID_type;
	int ID_type2;
	switch (expr->kind) {
		Type ID_type;
		int stackNo;
		case EXPR_ID:
			ID_type = getType(proc_id,expr->id);
			stackNo = getStackSlotNum(proc_id, expr->id);

			if(isParamRef(callee,paramNum)==0){

				printf("load r%d, %d\n", curr_reg,stackNo);
			}
			if(isParamRef(callee,paramNum)==1){

				printf("load_address r%d, %d\n", curr_reg,stackNo);
			}
			break;
		case EXPR_CONST:
			print_constant(expr->constant, curr_reg);
			break;
		case EXPR_BINOP:
			ID_type = getExprType(expr->e1, proc_id);
			ID_type2 = getExprType(expr->e2, proc_id);
			curr_reg = print_expr(expr->e1, curr_reg, proc_id);
			next_reg = print_expr(expr->e2, curr_reg + 1, proc_id);
			print_binop_string(expr->binop, curr_reg, next_reg, ID_type, ID_type2);
			break;
		case EXPR_RELOP:
			ID_type = getExprType(expr->e1, proc_id);
			ID_type2 = getExprType(expr->e2, proc_id);
			
			curr_reg = print_expr(expr->e1, curr_reg, proc_id);
			next_reg = print_expr(expr->e2, curr_reg + 1, proc_id);
			print_relop_string(expr->relop, curr_reg, next_reg, ID_type, ID_type2);
			break;
		case EXPR_UNOP:	
			ID_type = getExprType(expr->e1, proc_id);
			print_unop_string(expr->unop, curr_reg, ID_type);
			break;
			
			}
	return curr_reg;		
}

//Relational operators
void print_relop_string(int relop, int curr_reg, int next_reg, int ID1, int ID2){
	int ID = ID1 + ID2; //To see if it's float or int. float = 3/4.
	switch(relop){
		case RELOP_EQ:
			if (ID >= 3){
				printf("cmp_eq_real r%d, r%d, r%d\n", curr_reg, curr_reg, next_reg);
			}
			else{
				printf("cmp_eq_int r%d, r%d, r%d\n", curr_reg, curr_reg, next_reg);
			}
			break;
		case RELOP_NE:
			if (ID >= 3){
				printf("cmp_ne_real r%d, r%d, r%d\n", curr_reg, curr_reg, next_reg);}
			else{
				printf("cmp_ne_int r%d, r%d, r%d\n", curr_reg, curr_reg, next_reg);}
			break;
		case RELOP_LT:
			if (ID >= 3){
				printf("cmp_lt_real r%d, r%d, r%d\n", curr_reg, curr_reg, next_reg);}
			else{
				printf("cmp_lt_int r%d, r%d, r%d\n", curr_reg, curr_reg, next_reg);}
			break;
		case RELOP_GT:
			if (ID >= 3){
				printf("cmp_gt_real r%d, r%d, r%d\n", curr_reg, curr_reg, next_reg);}
			else{
				printf("cmp_gt_int r%d, r%d, r%d\n", curr_reg, curr_reg, next_reg);}
			break;
		case RELOP_LE:
			if (ID >= 3){
				printf("cmp_le_real r%d, r%d, r%d\n", curr_reg, curr_reg, next_reg);}
			else{
				printf("cmp_le_int r%d, r%d, r%d\n", curr_reg, curr_reg, next_reg);}
			break;
		case RELOP_GE:
			if (ID >= 3){
				printf("cmp_ge_real r%d, r%d, r%d\n",curr_reg, curr_reg, next_reg);}
			else{
				printf("cmp_ge_int r%d, r%d, r%d\n",curr_reg, curr_reg, next_reg);}
			break;
	}
}

//Unary operators
void print_unop_string(int unop, int reg, int ID){
	switch(unop){
		case UNOP_MINUS:
			if (ID == 2){
				printf("real_const r%d, -1\n", reg+1);
				printf("mul_real r%d, r%d, r%d \n", reg, reg, reg+1);
			}
			
			else{
				printf("int_const r%d, -1\n", reg+1);
				printf("mul_int r%d, r%d, r%d \n", reg, reg, reg+1);
			}
			break;
		case UNOP_NOT:
			printf("not r%d, r%d\n", reg, reg);
			break;
	
	}


}

static void print_args(Exprs exprs,int reg, char* procName, int paramNum,char* callee) {
	Expr first = exprs->first;
	Exprs rest = exprs->rest;
	if (first) {
			print_arg(first,reg,procName,paramNum,callee);
		if (rest) {
			print_args(rest,reg+1,procName,paramNum+1,callee);	
		}
	}
}
static void print_exprs(Exprs exprs,int reg, char* procName) {
	Expr first = exprs->first;
	Exprs rest = exprs->rest;
	if (first) {
			print_expr(first,reg,procName);
		if (rest) {
			print_exprs(rest,reg+1,procName);	
		}
	}
}

//Function calls
static void print_fncall(Stmt stmt, int l, char* procName) {
	printf("# fncall\n");
	if(stmt->info.fncall.args)
		print_args(stmt->info.fncall.args,0,procName,0,stmt->info.fncall.id);
	printf("call proc_%s\n", stmt->info.fncall.id);

}

//Constants for expressions
static void print_constant(Constant constant, int reg) {
	switch (constant.type) {
		case BOOL_CONSTANT:
			printf("int_const r%d, %d\n", reg, constant.val.bool_val);
			break;
		case INT_CONSTANT:
			printf("int_const r%d, %d\n", reg,constant.val.int_val);
			break;
		case FLOAT_CONSTANT:
			printf("real_const r%d, %s\n", reg, constant.val.float_val);
			break;
		case STRING_CONSTANT:
			//Should not be possible
			printf("string_const r%d, %s\n", reg, constant.val.str_val);
			break;
	}
}

static void print_indent(int level) {
	int i = 0;

    for (i = 0; i < level * 4; i++) {
		printf(" ");
	}
}

static Procs sort_procs(Procs procs) {
    Procs cur_node = procs;
    Proc tmp;

    while (cur_node != NULL) {
	    Procs next_node = cur_node->rest;

       	while (next_node) {
    		Proc cur_proc = cur_node->first;
    		Proc next_proc = next_node->first;

            if (strcmp(cur_proc->header->id, next_proc->header->id) > 0) {
                tmp = cur_proc;
                cur_node->first = next_proc;
                next_node->first = tmp;
            }
            next_node = next_node->rest;
        }

        cur_node = cur_node->rest;
    }
	return procs;
}
