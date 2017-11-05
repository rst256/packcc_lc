#ifndef _AST_H_
#define _AST_H_

#include <string.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
	FILE* input;
	const char * file_name;
	// struct {
	// 
	// } ;
} parser_t;

typedef enum ast_node_class {
	AST_NODE_EXPRESSION = 0,
	AST_NODE_TYPE_EXPR,
	AST_NODE_STATEMENT,
} ast_node_class_t;


typedef char ast_node_raw_type_t;
// typedef union ast_node_type {
// 	ast_node_class_t type;
// 	struct {
// 		ast_node_type_t class:3, subclass: 5;
// 	};
// } ast_node_type_t;

typedef enum ast_node_expression_type {
	AST_NODE_EXPRESSION_PRIMARY = 0,
	AST_NODE_EXPRESSION_UNOP,
	AST_NODE_EXPRESSION_BINOP,
	AST_NODE_EXPRESSION_CALL,
	AST_NODE_EXPRESSION_INDEX,
	AST_NODE_EXPRESSION_FIELD,

} ast_node_expression_type_t;

typedef struct ast_node_type {
	uint16_t type:3, subtype:4, species:9;
} ast_node_type_t;

typedef struct ast_node {
	// ast_node_class_t node_class:2;
	union {
		struct ast_node_type;
		struct ast_node_type node_type;
	};
			double number;
	union {
		union ast_expr {
			// struct ast_value {
			// 
			// } ;
			int ident;
			struct ast_expr_binop {
				struct ast_node * larg;
				struct ast_node * rarg;
			};
			struct ast_expr_unop {
				struct ast_node * arg;
			};
		};
	};
} ast_node_t;


ast_node_t * ast_binop_init(ast_node_t * node, ast_node_t *larg, int op, ast_node_t *rarg);

#endif // _AST_H_