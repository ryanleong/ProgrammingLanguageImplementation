
/*-----------------------------------------------------------------------
    A pretty-printer for Wiz programs.
    For use in the COMP90045 project 2014.

    Team:       Master Minds
    Members:    629204 Ye He
                547108 Yi Xia
                645452 Rongzuo Liu 
-----------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "std.h"
#include "codegen.h"
#include "symbol.h"

int loopcount = 0;
int ifcount = 0;

void pretty_prog(FILE *fp, Program prog) {
	Procs sorted_procs = sort_procs(prog->procs);
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
	printf("proc_%s", proc->header->id);
	int stack_count = getStackSize(proc->header->id);
	printf("\n");
	//print_header(proc->header);
	if (proc->decls) {
		//Get the number of declarations.
		printf("push_stack_frame %d", stack_count);
		printf("\n");
		//IF got int.
		//Do for float too
		printf("int_const r0, 0\n");
		printf("real_const r1, 0.0\n");
		printf("string_const r2, \"false\"");
		printf("\n");
		//Can get from tree?
		print_decls(proc->decls, 1, proc->header->id);
	}
	printf("\n");
	if (proc->body) {
		print_stmts(proc->body, 1, proc->header->id);
	}
	printf("end\n");

}

static void print_header(Header header) {
	printf("%s (", header->id);
	if (header->params) {
		print_params(header->params);
	}
	printf(")\n");
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

static void print_assign(Assign assign, int l) {
	//print_indent(l);
	printf("%s := ", assign.id);
	print_expr(assign.expr, 1);
	printf(";\n");
}


static void print_params(Params params) {
	Param first = params->first;
	Params rest = params->rest;
	if (first) {
		print_pram(first);
		if (rest) {
            printf(", ");
			print_params(rest);
		}
	}

}

static void print_pram(Param param) {
	switch (param->kind) {
		case REF:
			printf("ref ");
			break;
		case VAL:
			printf("val ");
			break;
	}

	switch (param->type) {
		case INT_PARAM:
			printf("int ");
			break;
		case FLOAT_PARAM:
			printf("float ");
			break;
		case BOOL_PARAM:
			printf("bool ");
			break;
	}
	printf("%s", param->id);
}

static void print_decl(Decl decl, int l, char* proc_id) {
	//print_indent(l);
	int slot;
	slot = getStackSlotNum(proc_id, decl->id);
	switch (decl->type) {
		case BOOL_TYPE:
			//printf("bool %s;\n", decl->id);
			printf("store %d, r2", slot);
			printf("\n");
			break;
		case INT_TYPE:
			//printf("int %s;\n", decl->id);
			printf("store %d, r0", slot);
			printf("\n");
			break;
		case FLOAT_TYPE:
			//printf("float %s;\n", decl->id);
			printf("store %d, r1", slot);
			printf("\n");
			break;
		case INT_ARRAY_TYPE:
			printf("int %s[", decl->id);
			print_intervals(decl->intervals);
			printf("];\n");
			break;
		case FLOAT_ARRAY_TYPE:
			printf("float %s[", decl->id);
			print_intervals(decl->intervals);
			printf("];\n");
			break;	
		case BOOL_ARRAY_TYPE:
			printf("bool %s[", decl->id);
			print_intervals(decl->intervals);
			printf("];\n");
			break;
	}
}

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

static Type getExprType(Expr expr, char* proc_id) {

	if (expr->kind == EXPR_CONST){

		// Return constant type
		if (expr->constant.type == FLOAT_CONSTANT) {
			return FLOAT_TYPE;	
		}
		else if (expr->constant.type == INT_CONSTANT){
			return INT_TYPE;
		}
	}

	else if (expr->kind == EXPR_ID) {
		// Return var type
		return getType(proc_id,expr->id);
	}

	else if (expr->kind == EXPR_BINOP) {
		Type e1Type = getExprType(expr->e1, proc_id);
		Type e2Type = getExprType(expr->e2, proc_id);

		// Compare and return expression type
		if (e1Type == INT_TYPE && e2Type == INT_TYPE) {
			return INT_TYPE;
		}
		else if ((e1Type == FLOAT_TYPE && e2Type == FLOAT_TYPE) || 
			(e1Type == INT_TYPE && e2Type == FLOAT_TYPE) ||
			(e1Type == FLOAT_TYPE && e2Type == INT_TYPE)) {
			
			return FLOAT_TYPE;
		}
	}
	
	return BOOL_TYPE;
}

static Type print_cond(Expr expr, char* proc_id, int reg, int stmt_type, char* label1, char* label2) {

	// TODO Check if e1 or e2 are expressions

	switch(expr->kind) {
		case EXPR_ID: {
			Type exprType = getType(proc_id,expr->id);
			printf("load r%d %d\n", reg, exprType);
			return exprType;
			break;
		}
		case EXPR_CONST:
			printf("load r%d %d\n", reg, expr->constant.val.int_val);

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
			// evaluate expression
			print_binop(expr, reg, proc_id);
			return getExprType(expr, proc_id);
			break;
		}
		case EXPR_RELOP: {
			// Print label if while loop
			if (stmt_type == 1) {
				// Add label to branch to at top of loop
				printf(":%s\n", label1);
			}

			Type e1Type = print_cond(expr->e1, proc_id, reg, stmt_type, label1, label2);
			reg++;
			Type e2Type = print_cond(expr->e2, proc_id, reg, stmt_type, label1, label2);

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



			// Check expression/var/const type
			switch(evalType) {
				case INT_TYPE:
					// check for RELOP type
					switch(expr->relop){
						case RELOP_EQ:
							printf("cmp_eq_int r%d r%d r%d\n", (reg-1), (reg-1), reg);
							break;
						case RELOP_NE:
							printf("cmp_ne_int r%d r%d r%d\n", (reg-1), (reg-1), reg);
							break;
						case RELOP_LT:
							printf("cmp_lt_int r%d r%d r%d\n", (reg-1), (reg-1), reg);
							break;
						case RELOP_GT:
							printf("cmp_gt_int r%d r%d r%d\n", (reg-1), (reg-1), reg);
							break;
						case RELOP_LE:
							printf("cmp_le_int r%d r%d r%d\n", (reg-1), (reg-1), reg);
							break;
						case RELOP_GE:
							printf("cmp_ge_int r%d r%d r%d\n", (reg-1), (reg-1), reg);
							break;
					}

					break;

				case FLOAT_TYPE:

					// check for RELOP type
					switch(expr->relop){
						case RELOP_EQ:
							printf("cmp_eq_real r%d r%d r%d\n", (reg-1), (reg-1), reg);
							break;
						case RELOP_NE:
							printf("cmp_ne_real r%d r%d r%d\n", (reg-1), (reg-1), reg);
							break;
						case RELOP_LT:
							printf("cmp_lt_real r%d r%d r%d\n", (reg-1), (reg-1), reg);
							break;
						case RELOP_GT:
							printf("cmp_gt_real r%d r%d r%d\n", (reg-1), (reg-1), reg);
							break;
						case RELOP_LE:
							printf("cmp_le_real r%d r%d r%d\n", (reg-1), (reg-1), reg);
							break;
						case RELOP_GE:
							printf("cmp_ge_real r%d r%d r%d\n", (reg-1), (reg-1), reg);
							break;
					}

					break;
				case BOOL_TYPE:
					break;
			}

			if (stmt_type == 0) {
				printf("branch_on_false r%d %s\n", (reg-1), label1);
			}
			else if (stmt_type == 1) {
				// Branch away if false
				printf("branch_on_false r%d %s\n", (reg-1), label2);
			}

			

			break; 
		}
	}

	return -1;
}

static Type print_while(Expr expr, char* proc_id, int reg) {
	printf("Here\n");
	return -1;
}

static void print_stmt(Stmt stmt, int l, char* proc_id) {
	switch (stmt->kind) {
		case STMT_ASSIGN:
			print_assign(stmt->info.assign, l);
			break;		
		case STMT_ASSIGN_ARRAY:
			//print_assign_array(stmt->info.assign, l);
			break;
		case STMT_COND:
			//print_cond(stmt->info.cond, l);
			print_cond(stmt->info.cond.cond, proc_id, 0, 0, "if_is_false", "");

			// Print then statements
			print_stmts(stmt->info.cond.then_branch, l, proc_id);

			// Do False
			printf(":if_is_false\n");

			// Print else statements
			if (stmt->info.cond.else_branch != NULL) {
				print_stmts(stmt->info.cond.else_branch, l, proc_id);
			}
			
			break;
		case STMT_WHILE:
			// print while condition
			print_cond(stmt->info.loop.cond, proc_id, 0, 1, "top_of_loop", "loop_is_false");

			// Print statements
			print_stmts(stmt->info.loop.body, l, proc_id);

			// Print unconditional branch
			printf("branch_uncond %s\n", "top_of_loop");

			// Print end loop label
			printf(":loop_is_false\n");

			break;
		case STMT_READ:
			print_read(stmt, l, proc_id);
			break;		
		case STMT_READ_ARRAY:
			print_read_array(stmt, l);
			break;
		case STMT_WRITE:
			print_write(stmt, l, proc_id);
			break;
		case STMT_FNCALL:
			print_fncall(stmt, l);
			break;
	}
}

static void print_read(Stmt stmt, int l, char* proc_id) {
	//print_indent(l);
	printf("#read");
	printf("\n");
	//Get the type and stack number from the id
	Type ID_type;
	int stackNo;
	ID_type = getType(proc_id,stmt->info.read);
	stackNo = getStackSlotNum(proc_id, stmt->info.read);
	switch (ID_type){			//placeholder			
		case BOOL_TYPE:
		//printf("bool %s;\n", decl->id);
			printf("call_builtin read_bool"); //with a function
			printf("\n");
			printf("store %d, r0", stackNo);
			printf("\n");
			break;
		case INT_TYPE:
			printf("call_builtin read_int \n"); //with a function
			printf("store %d, r0 \n", stackNo);
			break;
		case FLOAT_TYPE:
			printf("call_builtin read_real \n"); //with a function
			printf("store %d, r0 \n", stackNo);
			break; 
	}
	//printf("read %s", stmt->info.read);
	//printf(";\n");
}

static void print_read_array(Stmt stmt, int l) {
	print_indent(l);
	printf("read %s", stmt->info.array.id);
	printf("[");
	print_exprs(stmt->info.array.exprs);
	printf("];\n");
}

static void print_write(Stmt stmt, int l, char* proc_id) {
	//print_indent(l);
	printf("# write\n");
	print_write_expr(stmt->info.write, 1, proc_id);
	printf("\n");
}

static void print_write_expr(Expr expr, int depth, char* proc_id) {
	switch (expr->kind) {
		int ID_type = 0;
		int ID_type2 = 0;
		int stackNo;
		int reg; // for Binop use
		int reg1;
		case EXPR_ID:
			ID_type = getType(proc_id,expr->id);
			stackNo = getStackSlotNum(proc_id, expr->id);
			//Look up id in the symbol table to get the type and stack
			switch (ID_type){			//placeholder			
				case BOOL_TYPE:
				//printf("bool %s;\n", decl->id);
				printf("string_const r0, %d", stackNo); //Change the value to lookup for value
				printf("\n");
				printf("call_builtin print_bool"); //with a function
				printf("\n");
				break;
				case INT_TYPE:
				//printf("int %s;\n", decl->id);
				printf("load r0, %d \n", stackNo);
				printf("call_builtin print_int \n"); //with a function
				break;
				case FLOAT_TYPE:
				//printf("float %s;\n", decl->id);
				printf("load r0, %d \n", stackNo);
				printf("call_builtin print_real \n"); //with a function
				break; 
				}
			//printf("%s", expr->id);
			break;
		case EXPR_CONST:
			print_write_constant(expr->constant);
			break;
		case EXPR_BINOP:
			reg = print_binop(expr->e1, 0, proc_id);
			reg1 = print_binop(expr->e2, 1, proc_id);
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
			reg =  print_binop(expr->e1, 0, proc_id);
			reg1 = print_binop(expr->e2, 1, proc_id);
			ID_type = getExprType(expr->e1, proc_id);
			ID_type2 = getExprType(expr->e2, proc_id);
			print_relop_string(expr->relop, reg, reg1, ID_type, ID_type2);
			printf("call_builtin print_bool \n");
			break;
		case EXPR_UNOP:
			reg =  print_binop(expr->e1, 0, proc_id);
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
	}
}

void print_binop_string(int binop, int curr_reg, int next_reg, int ID1, int ID2){
	int ID = ID1 + ID2; //To see if it's float or int. float = 3/4.
	switch(binop){
		case BINOP_ADD:
			if (ID >= 3){ 
				printf("add_real r%d r%d r%d \n", curr_reg, curr_reg, next_reg);
			}
			else{
				printf("add_int r%d r%d r%d \n", curr_reg, curr_reg, next_reg);
			}
			break;
		case BINOP_SUB:
			if (ID >= 3){
				printf("sub_real r%d r%d r%d \n", curr_reg, curr_reg, next_reg);
				}
			else{
				printf("sub_int r%d r%d r%d \n", curr_reg, curr_reg, next_reg);
			}
			break;
		case BINOP_MUL:
			if (ID >= 3){
				printf("mul_real r%d r%d r%d \n", curr_reg, curr_reg, next_reg);
				}
			else{
				printf("mul_int r%d r%d r%d \n", curr_reg, curr_reg, next_reg);
				}
			break;
		case BINOP_DIV:
			if (ID >= 3){
				printf("div_real r%d r%d r%d \n", curr_reg, curr_reg, next_reg);
				}
			else{
				printf("div_int r%d r%d r%d \n", curr_reg, curr_reg, next_reg);
				}
			break;
		case BINOP_AND:
			printf("and r%d r%d r%d \n", curr_reg, curr_reg, next_reg);
			break;
		case BINOP_OR:
			printf("or r%d r%d r%d \n", curr_reg, curr_reg, next_reg);
			break;		
	}
}

static void print_write_constant(Constant constant) {
	switch (constant.type) {
		case BOOL_CONSTANT:
			//printf("%s", constant.val.bool_val ? "true" : "false");
			printf("string_const r0, %s", constant.val.bool_val ? "true" : "false");
			printf("\n");
			printf("call_builtin print_bool"); //with a function
			printf("\n");
			break;
		case INT_CONSTANT:
			//printf("%d", constant.val.int_val);
			printf("int_const r0, %d", constant.val.int_val);
			printf("\n");
			printf("call_builtin print_int"); //with a function
			printf("\n");
			break;
		case FLOAT_CONSTANT:
			//printf("%s", constant.val.float_val);
			printf("real_const r0, %s", constant.val.float_val);
			printf("\n");
			printf("call_builtin print_real"); //with a function
			printf("\n");
			break;
		case STRING_CONSTANT:
			//printf("%s", constant.val.str_val);
			printf("string_const r0, %s", constant.val.str_val);
			printf("\n");
			printf("call_builtin print_string"); //with a function
			printf("\n");
			break;
	}
}

int print_binop(Expr expr, int reg, char* proc_id) {
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
			//Look up id in the symbol table to get the type and stack
			switch (ID_type){						
				case BOOL_TYPE:
					printf("string_const r%d, %d\n", curr_reg,stackNo); //Change the value to lookup for value
					break;
				case INT_TYPE:
					printf("load r%d, %d \n", curr_reg,stackNo);
					break;
				case FLOAT_TYPE:
					printf("load r%d, %d \n", curr_reg,stackNo);
					break;
					}
			break;
		case EXPR_CONST:
			print_constant(expr->constant, curr_reg);
			break;
		case EXPR_BINOP:
			ID_type = getExprType(expr->e1, proc_id);
			ID_type2 = getExprType(expr->e2, proc_id);
			
			curr_reg = print_binop(expr->e1, curr_reg, proc_id);
			next_reg = print_binop(expr->e2, curr_reg + 1, proc_id);
			print_binop_string(expr->binop, curr_reg, next_reg, ID_type, ID_type2);
			break;
		case EXPR_RELOP:
			ID_type = getExprType(expr->e1, proc_id);
			ID_type2 = getExprType(expr->e2, proc_id);
			
			curr_reg = print_binop(expr->e1, curr_reg, proc_id);
			next_reg = print_binop(expr->e2, curr_reg + 1, proc_id);
			print_relop_string(expr->relop, curr_reg, next_reg, ID_type, ID_type2);
			break;
		case EXPR_UNOP:	
			ID_type = getExprType(expr->e1, proc_id);
			print_unop_string(expr->unop, curr_reg, ID_type);
			break;
			
			}
	return curr_reg;		
}

void print_relop_string(int relop, int curr_reg, int next_reg, int ID1, int ID2){
	int ID = ID1 + ID2; //To see if it's float or int. float = 3/4.
	switch(relop){
		case RELOP_EQ:
			if (ID >= 3){
				printf("cmp_eq_real r%d r%d r%d\n", curr_reg, curr_reg, next_reg);
			}
			else{
				printf("cmp_eq_int r%d r%d r%d\n", curr_reg, curr_reg, next_reg);
			}
			break;
		case RELOP_NE:
			if (ID >= 3){
				printf("cmp_ne_real r%d r%d r%d\n", curr_reg, curr_reg, next_reg);}
			else{
				printf("cmp_ne_int r%d r%d r%d\n", curr_reg, curr_reg, next_reg);}
			break;
		case RELOP_LT:
			if (ID >= 3){
				printf("cmp_lt_real r%d r%d r%d\n", curr_reg, curr_reg, next_reg);}
			else{
				printf("cmp_lt_int r%d r%d r%d\n", curr_reg, curr_reg, next_reg);}
			break;
		case RELOP_GT:
			if (ID >= 3){
				printf("cmp_gt_real r%d r%d r%d\n", curr_reg, curr_reg, next_reg);}
			else{
				printf("cmp_gt_int r%d r%d r%d\n", curr_reg, curr_reg, next_reg);}
			break;
		case RELOP_LE:
			if (ID >= 3){
				printf("cmp_le_real r%d r%d r%d\n", curr_reg, curr_reg, next_reg);}
			else{
				printf("cmp_le_int r%d r%d r%d\n", curr_reg, curr_reg, next_reg);}
			break;
		case RELOP_GE:
			if (ID >= 3){
				printf("cmp_ge_real r%d r%d r%d\n",curr_reg, curr_reg, next_reg);}
			else{
				printf("cmp_ge_int r%d r%d r%d\n",curr_reg, curr_reg, next_reg);}
			break;
	}
}

void print_unop_string(int unop, int reg, int ID){
	switch(unop){
		case UNOP_MINUS:
			if (ID == 2){
				printf("real_const r%d, -1\n", reg+1);
				printf("mul_real r%d r%d r%d \n", reg, reg, reg+1);
			}
			
			else{
				printf("int_const r%d, -1\n", reg+1);
				printf("mul_int r%d r%d r%d \n", reg, reg, reg+1);
			}
			break;
		case UNOP_NOT:
			printf("not r%d, r%d\n", reg, reg);
			break;
	
	}


}

static void print_expr(Expr expr, int depth) {
	switch (expr->kind) {
		case EXPR_ID:
			printf("%s", expr->id);
			break;
		case EXPR_CONST:
			//print_constant(expr->constant);
			break;
		case EXPR_BINOP:
		    if (depth != 1)
		    	printf("(");
			print_expr(expr->e1, depth + 1);
			printf(" %s ", binop_names[expr->binop]);
			print_expr(expr->e2, depth + 1);
			if (depth != 1)
				printf(")");
			break;
		case EXPR_UNOP:
			printf("%s ", unop_names[expr->unop]);
			print_expr(expr->e1, depth + 1);
			break;
		case EXPR_RELOP:
			print_expr(expr->e1, depth);
			printf(" %s ", relop_names[expr->relop]);
			print_expr(expr->e2, depth);
			break;
		case EXPR_ARRAY:
			printf("%s", expr->id);
			printf("[");
			print_exprs(expr->es);
			printf("]");
			break;
	}
}

static void print_exprs(Exprs exprs) {
	Expr first = exprs->first;
	Exprs rest = exprs->rest;
	if (first) {
			print_expr(first, 1);
		if (rest) {
			printf(", ");
			print_exprs(rest);	
		}
	}
}


static void print_fncall(Stmt stmt, int l) {
	print_indent(l);
	printf("%s", stmt->info.fncall.id);
	printf("(");
	print_exprs(stmt->info.fncall.args);
	printf(");\n");
}

static void print_constant(Constant constant, int reg) {
	switch (constant.type) {
		case BOOL_CONSTANT:
			//printf("%s", constant.val.bool_val ? "true" : "false");
			printf("string_const r%d, %s\n", reg, constant.val.bool_val ? "true" : "false");
			break;
		case INT_CONSTANT:
			printf("int_const r%d, %d\n", reg,constant.val.int_val);
			break;
		case FLOAT_CONSTANT:
			printf("real_const r%d, %s\n", reg, constant.val.float_val);
			break;
		case STRING_CONSTANT:
			//printf("%s", constant.val.str_val);
			//Should not be possible?
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
