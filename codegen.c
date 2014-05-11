#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "std.h"



//Need input form before going through
void print_code(FILE *fp, Program prog) {
	Procs sorted_procs = sort_procs(prog->procs);
	print_procs(sorted_procs);
}

static void print_procs(Procs procs) {
	Proc first = procs->first;  //Change to whatever is given
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
	printf("call ");
	//print_header(proc->header);
	if (proc->decls) {
		print_decls(proc->decls, 1);
	}
	printf("\n");
	if (proc->body) {
		print_stmts(proc->body, 1);
	}
	printf("end\n");

}
static void print_decls(Decls decls, int l) {
	//Get the number of declarations.
	int count = 0;
	printf("push_stack_frame %d", count);
	printf("\n");
	//IF got int.
	//Do for float too
	printf("int_const r0, 0");
	printf("\n");
	//Can get from tree?
	Decl first = decls->first;
	Decls rest = decls->rest;
	
	
	if (first) {
		print_decl(first, l);
	}
	if (rest) {
		print_decls(rest, l);
	}
}

static void print_decl(Decl decl, int l) {
	print_indent(l);
	switch (decl->type) {
		case BOOL_TYPE:
			printf("bool %s;\n", decl->id);
			break;
		case INT_TYPE:
			printf("int %s;\n", decl->id);
			break;
		case FLOAT_TYPE:
			printf("float %s;\n", decl->id);
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

static void print_header(Header header) {
	printf("%s (", header->id);
	if (header->params) {
		print_params(header->params);
	}
	printf(")\n");
}


static void print_assign(Assign assign, int l) {
	print_indent(l);
	printf("%s := ", assign.id);
	print_expr(assign.expr, 1);
	printf(";\n");
}

static void print_assign_array(Assign assign, int l) {
	print_indent(l);
	printf("%s", assign.id);
	printf("[");
	print_exprs(assign.exprs);
	printf("] := ");
	print_expr(assign.expr, 1);
	printf(";\n");
}

static void print_cond(Cond cond_stmt, int l) {
	print_indent(l);
	printf("if ");
	print_expr(cond_stmt.cond, 1);
	printf(" then\n");
	print_stmts(cond_stmt.then_branch, l + 1);
	if (cond_stmt.else_branch) {
		print_indent(l);
		printf("else\n");
		print_stmts(cond_stmt.else_branch, l + 1);
	}
	print_indent(l);
	printf("fi\n");
}

static void print_while(While while_stmt, int l) {
	print_indent(l);
	printf("while ");
	print_expr(while_stmt.cond, 1);
	printf(" do\n");
	if (while_stmt.body) {
		print_stmts(while_stmt.body, l + 1);
	}
	print_indent(l);
	printf("od\n");
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

static void print_stmts(Stmts stmts, int l) {
	Stmt first = stmts->first;
	Stmts rest = stmts->rest;
	if (first) {
		print_stmt(first, l);
	}
	if (rest) {
		print_stmts(rest, l);
	}
}

static void print_stmt(Stmt stmt, int l) {
	switch (stmt->kind) {
		case STMT_ASSIGN:
			print_assign(stmt->info.assign, l);
			break;		
		case STMT_ASSIGN_ARRAY:
			print_assign_array(stmt->info.assign, l);
			break;
		case STMT_COND:
			print_cond(stmt->info.cond, l);
			break;
		case STMT_WHILE:
			print_while(stmt->info.loop, l);
			break;
		case STMT_READ:
			print_read(stmt, l);
			break;		
		case STMT_READ_ARRAY:
			print_read_array(stmt, l);
			break;
		case STMT_WRITE:
			print_write(stmt, l);
			break;
		case STMT_FNCALL:
			print_fncall(stmt, l);
			break;
	}
}

static void print_read(Stmt stmt, int l) {
	print_indent(l);
	printf("read %s", stmt->info.read);
	printf(";\n");
}

static void print_read_array(Stmt stmt, int l) {
	print_indent(l);
	printf("read %s", stmt->info.array.id);
	printf("[");
	print_exprs(stmt->info.array.exprs);
	printf("];\n");
}

static void print_write(Stmt stmt, int l) {
	print_indent(l);
	printf("int_const r0, %s", info.write);
	printf("float_const r0, %s", info.write);
	printf("string_const r0, %s", info.write);
	print_expr(stmt->info.write, 1);
	printf("call_builtin") //with a function
	printf(";\n");
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