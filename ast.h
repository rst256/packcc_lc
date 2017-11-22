#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <malloc.h> 

#include "ident.h"
#include "scope.h"

#ifndef _AST_H_
#define _AST_H_

typedef struct position_tag {
	int line, col;
} position_t;

typedef struct {
	FILE* input;
	const char * file_name;
	union {
		struct position_tag;
		struct position_tag begin_pos;
	};
	struct position_tag end_pos;
	int lines_count;
	int pos;
	scope_t* scope;
	int lines[2048];
} parser_t;


#define LC_MAX_FUNCTION_ARGS 30
#define LC_MAX_BLOCK_ITEMS 3000

enum asn_type {
	asn_null = 0,
	expr_integer, expr_string, expr_float, 
	expr_ident, expr_field, expr_index, 
	expr_binop, expr_call, 
	block,
	stat_assign, stat_define,
	stat_if, stat_if_then_else,
	type_expr,
};

typedef struct asn_list {
	int count;
	struct asn ** nodes;
	scope_t* scope;
} asn_list_t;

typedef unsigned int parser_pos_t;

struct asn_shared {
	int type:28, is_statement:1;
	parser_pos_t begin_pos;
	parser_pos_t end_pos;
};

struct asn_expr_primary {
	double number;
};

struct asn_expr_float {
	double float_num;
};

struct asn_expr_string {
	const char* string;
};

struct asn_expr_binop {
        struct asn * larg;
        char op;
        struct asn * rarg;
};

struct asn_expr_call {
	int args_count;
	struct asn* func;
	struct asn** args;
};

struct asn_expr_field {
	struct asn* object;
	ident_t* field;
};

struct asn_expr_index {
	struct asn* array;
	struct asn* index;
};

struct asn_stat_assign {
	struct asn* lvalue;
	struct asn* rvalue;
};

struct asn_stat_if {
	struct asn* condition;
	struct asn* then_block;
	struct asn* else_block;
};

struct asn_stat_if_then_else {
	struct asn* condition_blocks;
	struct asn* else_block;
};

struct asn_stat_define {
	struct asn* def_type;
	ident_t* def_ident;
	struct asn* init_value;
};

enum asn_type_expr_type {
	te_primary = 0, te_pointer, te_array, 
};

struct asn_type_expr {
	int is_const:1, te_type:3, len:20;
	union {
		ident_t * base_type_name;
		struct asn * base_type;
	};
};


typedef struct asn {
	struct asn_shared;
	union {
		ident_t* ident;
		struct asn_expr_primary;
		struct asn_expr_binop;
		struct asn_expr_call;
		struct asn_stat_assign;
		struct asn_expr_field;
		struct asn_expr_index;
		struct asn_expr_string;
		struct asn_expr_float;
		struct asn_stat_if;
		struct asn_list;
		struct asn_type_expr;
		struct asn_stat_define;
		//struct asn_expr_unop;
	};
} asn_t;

int get_line(parser_t* p, int pos);
int get_col(parser_t* p, int pos);

#define _dump_asn(ASN, BUFF, SIZE, ...) __dump_asn(ASN, BUFF, SIZE)
#define dump_asn(...) _dump_asn(__VA_ARGS__, alloca(2048), 2048)
const char * __dump_asn(asn_t *n, char *buff, size_t size);

#define _print_asn(ASN, LEVEL, ...) __print_asn(ASN, LEVEL)
#define print_asn(...) _print_asn(__VA_ARGS__, 0)
void __print_asn(asn_t *n, int level);

asn_t * binop(asn_t * l, const char *op, asn_t * r, parser_t * auxil, const char* _0);

#endif // _AST_H_