#include "ast.h"

int get_line(parser_t* p, int pos){ 
	if(p->pos < pos) return 0;
	for(int i = 0; i<p->lines_count; i++) if(p->lines[i] > pos) return i+1;
	return p->lines_count+1; 
}

int get_col(parser_t* p, int pos){ 
	if(p->pos < pos) return 0;
	int i;
	for(i = 0; i<p->lines_count; i++) if(p->lines[i] > pos) break;
	return ( i ? pos - p->lines[i-1] : pos ) + 1; 
}

const char * __dump_asn(asn_t *n, char *buff, size_t size){
	int shift;
	if(!n) return "";
	switch(n->type){
		case asn_null:
			shift = snprintf(buff, size, " ");
			break;
		case expr_integer:
			shift = snprintf(buff, size, "%g", n->number);
			break;
		case expr_float:
			shift = snprintf(buff, size, "%g", n->float_num);
			break;
		case expr_string:
			shift = snprintf(buff, size, "\"%s\"", n->string);
			break;
		case expr_ident:
			shift = snprintf(buff, size, "%s", ident_name(n->ident));
			break;
		case expr_field:
			shift = snprintf(buff, size, "%s.%s", dump_asn(n->object), ident_name(n->field));
			break;
		case expr_index:
			shift = snprintf(buff, size, "%s[%s]", dump_asn(n->array), dump_asn(n->index)); 
			break;
		case expr_binop:
			shift = snprintf(buff, size, "(%s %c %s)", dump_asn(n->larg),  n->op, dump_asn(n->rarg));
			break;
		case expr_call:
			shift = snprintf(buff, size, "%s()", dump_asn(n->func)); 
			/*/for(int i = 0; i<n->args_count; i++){ print_asn(n->args[i]); if(i+1<n->args_count) shift = snprintf(buff, size, ", "); }
			# shift = snprintf(buff, size, ")"); */
			break;
		case stat_assign:
			shift = snprintf(buff, size, "%s = %s", dump_asn(n->lvalue), dump_asn(n->rvalue));
			break;
		default:
			shift = snprintf(buff, size, "?[%d]?", n->type);
		break;
	}
	if(n->is_statement) snprintf(buff+shift, size-shift, ";"); 
	return buff;
}


void __print_asn(asn_t *n, int level){
	if(!n) return;
//	if(n->is_statement) 
for(int i = 0; i<level; i++) printf("\t");
	switch(n->type){
		case asn_null:
			printf(" ");
			break;
		case expr_integer:
			printf("%g", n->number);
			break;
		case expr_float:
			printf("%g", n->float_num);
			break;
		case expr_string:
			printf("\"%s\"", n->string);
			break;
		case expr_ident:
			printf("%s", ident_name(n->ident));
			break;
		case expr_field:
			print_asn(n->object); printf(".%s", ident_name(n->field));
			break;
		case expr_index:
			print_asn(n->array); printf("["); print_asn(n->index); printf("]"); 
			break;
		case expr_binop:
			printf("("); print_asn(n->larg); printf(" %c ", n->op); print_asn(n->rarg); printf(")"); 
			break;
		case expr_call:
			print_asn(n->func); printf("("); 
			for(int i = 0; i<n->args_count; i++){ print_asn(n->args[i]); if(i+1<n->args_count) printf(", "); }
			printf(")"); 
			break;
		case block:
			for(int i = 0; i<n->count; i++){ 
				print_asn(n->nodes[i], (i ? level : 0)); 
				if(i+1<n->count) printf("\n"); 
			}
			break;
		case stat_assign:
			print_asn(n->lvalue); printf(" = "); print_asn(n->rvalue);
			break;
		case stat_if:
			printf("if "); print_asn(n->condition); 
			printf(" then\n"); print_asn(n->then_block, level+1);
			printf("\n"); for(int i = 0; i<level; i++) printf("\t");
			printf("end");
			break;
		case stat_if_then_else:
			printf("if "); print_asn(n->condition); 
			printf(" then\n"); print_asn(n->then_block, level+1);
			printf("\n"); for(int i = 0; i<level; i++) printf("\t");
			printf("else\n"); print_asn(n->else_block, level+1); 
			printf("\n"); for(int i = 0; i<level; i++) printf("\t");
			printf("end");
			break;
		case type_expr:
			if(n->is_const && n->te_type!=te_pointer) printf("const ");
			print_asn(n->base_type); 
			if(n->is_const && n->te_type==te_pointer) printf(" const");
			if(n->te_type==te_pointer) printf("*");
			break;
		case stat_define:
			print_asn(n->def_type); 
			printf(" %s", ident_name(n->def_ident));
			if(n->init_value){
				printf(" = "); print_asn(n->init_value);
			}
			break;
		default:
			printf("?[%d]?", n->type);
		break;
	}
	//if(n->is_statement) printf(";"); 
}


asn_t * binop(asn_t * l, const char *op, asn_t * r, parser_t * auxil, const char* _0){
	asn_t * __;
	if(l->type==expr_integer && r->type==expr_integer){
		__ = malloc(sizeof(asn_t));
		__->type = expr_integer; __->is_statement = 0; 
		switch(op[0]){
		  case '-': __->number = l->number - r->number; break;
		  case '+': __->number = l->number + r->number; break;
		  case '/': __->number = l->number / r->number; break;
		  case '*': __->number = l->number * r->number; break;
		  default:
		    
		    break;
		}
	}else{
		__ = malloc(sizeof(asn_t));
		__->type = expr_binop; __->is_statement = 0; 
		__->larg=l; __->rarg=r; __->op=op[0]; 
	}
	return __;
}
