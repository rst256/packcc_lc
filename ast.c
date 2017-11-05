#include "ast.h"

ast_node_t * ast_node_init(ast_node_t * node, ast_node_class_t type, uint16_t subtype, uint16_t species){
	memset(node, 0, sizeof(*node));
	node->node_type = (ast_node_type_t){ type, subtype, species };
	return node;
}

ast_node_t * ast_binop_init(ast_node_t * node, ast_node_t *larg, int op, ast_node_t *rarg){
	ast_node_init(node, AST_NODE_EXPRESSION, AST_NODE_EXPRESSION_BINOP, op);
	node->larg = larg;
	node->rarg = rarg;
	return node;
}