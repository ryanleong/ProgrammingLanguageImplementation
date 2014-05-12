
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
			break;
		case STMT_WHILE:
			//print_while(stmt->info.loop, l);
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
		Type ID_type;
		int stackNo;
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
			print_expr(expr->e1, depth + 1);
			printf(" %s ", binop_names[expr->binop]);
			print_expr(expr->e2, depth + 1);
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
			printf("int_const r0, %f", constant.val.float_val);
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

static void print_expr(Expr expr, int depth) {
	switch (expr->kind) {
		case EXPR_ID:
			printf("%s", expr->id);
			break;
		case EXPR_CONST:
			print_constant(expr->constant);
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

static void print_constant(Constant constant) {
	switch (constant.type) {
		case BOOL_CONSTANT:
			printf("%s", constant.val.bool_val ? "true" : "false");
			break;
		case INT_CONSTANT:
			printf("%d", constant.val.int_val);
			break;
		case FLOAT_CONSTANT:
			printf("%s", constant.val.float_val);
			break;
		case STRING_CONSTANT:
			printf("%s", constant.val.str_val);
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
