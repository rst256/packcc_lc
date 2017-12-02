/*
 * PackCC: a packrat parser generator for C.
 *
 * Copyright (c) 2014 Arihiro Yoshida. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * The algorithm is based on the paper "Packrat Parsers Can Support Left Recursion"
 * authored by A. Warth, J. R. Douglass, and T. Millstein.
 *
 * The specification is determined by referring to peg/leg developed by Ian Piumarta.
 */
#ifndef _PACKCC_H
#define _PACKCC_H

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define snprintf _snprintf
#define unlink _unlink
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>


#define VERSION "1.2.0"

#ifndef BUFFER_INIT_SIZE
#define BUFFER_INIT_SIZE 256
#endif
#ifndef ARRAY_INIT_SIZE
#define ARRAY_INIT_SIZE 2
#endif

typedef enum bool_tag {
    FALSE = 0,
    TRUE
} bool_t;

typedef struct char_array_tag {
    char *buf;
    int max;
    int len;
} char_array_t;

typedef enum node_type_tag {
    NODE_RULE = 0,
    NODE_REFERENCE,
    NODE_STRING,
    NODE_CHARCLASS,
    NODE_QUANTITY,
    NODE_PREDICATE,
    NODE_SEQUENCE,
    NODE_ALTERNATE,
    NODE_CAPTURE,
    NODE_EXPAND,
    NODE_ACTION,
    NODE_ERROR,
} node_type_t;

typedef struct node_tag node_t;

typedef struct node_array_tag {
    node_t **buf;
    int max;
    int len;
} node_array_t;

typedef struct node_const_array_tag {
    const node_t **buf;
    int max;
    int len;
} node_const_array_t;

typedef struct node_hash_table_tag {
    const node_t **buf;
    int max;
    int mod;
} node_hash_table_t;

typedef struct node_rule_tag {
    char *name;
    node_t *expr;
    int ref; /* mutable */
    node_const_array_t vars;
    node_const_array_t capts;
    node_const_array_t codes;
    int line;
    int col;
} node_rule_t;

typedef struct node_reference_tag {
    char *var; /* NULL if no variable name */
    int index;
    char *name;
    const node_t *rule;
    int line;
    int col;
} node_reference_t;

typedef struct node_string_tag {
    char *value;
} node_string_t;

typedef struct node_charclass_tag {
    char *value; /* NULL means any character */
} node_charclass_t;

typedef struct node_quantity_tag {
    int min;
    int max;
    node_t *expr;
} node_quantity_t;

typedef struct node_predicate_tag {
    bool_t neg;
    node_t *expr;
} node_predicate_t;

typedef struct node_sequence_tag {
    node_array_t nodes;
} node_sequence_t;

typedef struct node_alternate_tag {
    node_array_t nodes;
} node_alternate_t;

typedef struct node_capture_tag {
    node_t *expr;
    int index;
} node_capture_t;

typedef struct node_expand_tag {
    int index;
    int line;
    int col;
} node_expand_t;

typedef struct node_action_tag {
    char *value;
    int index;
    node_const_array_t vars;
    node_const_array_t capts;
} node_action_t;

typedef struct node_error_tag {
    node_t *expr;
    char *value;
    int index;
    node_const_array_t vars;
    node_const_array_t capts;
} node_error_t;

typedef union node_data_tag {
    node_rule_t      rule;
    node_reference_t reference;
    node_string_t    string;
    node_charclass_t charclass;
    node_quantity_t  quantity;
    node_predicate_t predicate;
    node_sequence_t  sequence;
    node_alternate_t alternate;
    node_capture_t   capture;
    node_expand_t    expand;
    node_action_t    action;
    node_error_t     error;
} node_data_t;

struct node_tag {
    node_type_t type;
    node_data_t data;
};

typedef struct context_tag {
    char *iname;
    char *sname;
    char *hname;
    char *lname;
    FILE *ifile;
    FILE *sfile;
    FILE *hfile;
    FILE *lfile;
    char *hid;
    char *vtype;
    char *atype;
    char *prefix;
    bool_t debug;
    int errnum;
    int linenum;
    int linepos;
    int bufpos;
    char_array_t buffer;
    node_array_t rules;
    node_hash_table_t rulehash;
} context_t;

typedef struct generate_tag {
    FILE *stream;
    const node_t *rule;
    int label;
} generate_t;

typedef enum string_flag_tag {
    STRING_FLAG__NONE = 0,
    STRING_FLAG__NOTEMPTY = 1,
    STRING_FLAG__NOTVOID = 2,
    STRING_FLAG__IDENTIFIER = 4,
} string_flag_t;

typedef enum code_reach_tag {
    CODE_REACH__BOTH = 0,
    CODE_REACH__ALWAYS_SUCCEED = 1,
    CODE_REACH__ALWAYS_FAIL = -1
} code_reach_t;

 // const char *g_cmdname = "packcc"; /* replaced later with actual one */





 size_t strnlen(const char *str, size_t maxlen);
 int print_error(const char *format, ...);
 FILE *fopen_rb_e(const char *path);
 FILE *fopen_wt_e(const char *path);
 void *malloc_e(size_t size);
 void *realloc_e(void *ptr, size_t size);
 char *strdup_e(const char *str);
 char *strndup_e(const char *str, size_t len);
 bool_t is_filled_string(const char *str);
 bool_t is_identifier_string(const char *str);
 bool_t is_pointer_type(const char *str);
 bool_t unescape_string(char *str);
 const char *escape_character(char ch, char (*buf)[5]);
 void remove_heading_blank(char *str);
 void remove_trailing_blank(char *str);
 void make_header_identifier(char *str);
 void write_characters(FILE *stream, char ch, size_t len);
 void write_text(FILE *stream, const char *ptr, size_t len);
 void write_code_block2(FILE *stream, const char *ptr, size_t len, int indent);
 void write_code_block(FILE *stream, const char *ptr, size_t len, int indent);
 const char *extract_filename(const char *path);
 const char *extract_fileext(const char *path);
 char *replace_fileext(const char *path, const char *ext);
 char *add_fileext(const char *path, const char *ext);
 int hash_string(const char *str);
 int populate_bits(int x);
 void char_array__init(char_array_t *array, int max);
 void char_array__add(char_array_t *array, char ch);
 void char_array__term(char_array_t *array);
 void node_array__init(node_array_t *array, int max);
 void node_array__add(node_array_t *array, node_t *node);
 void node_array__term(node_array_t *array);
 void node_const_array__init(node_const_array_t *array, int max);
 void node_const_array__add(node_const_array_t *array, const node_t *node);
 void node_const_array__clear(node_const_array_t *array);
 void node_const_array__copy(node_const_array_t *array, const node_const_array_t *src);
 void node_const_array__term(node_const_array_t *array);
 context_t *create_context(const char *iname, const char *oname, bool_t debug);
 node_t *create_node(node_type_t type);
 void destroy_node(node_t *node);
 void destroy_context(context_t *ctx);
 void make_rulehash(context_t *ctx);
 const node_t *lookup_rulehash(const context_t *ctx, const char *name);
 void link_references(context_t *ctx, node_t *node);
 void verify_variables(context_t *ctx, node_t *node, node_const_array_t *vars);
 void verify_captures(context_t *ctx, node_t *node, node_const_array_t *capts);
 void dump_node(context_t *ctx, const node_t *node);
 int refill_buffer(context_t *ctx, int num);
 void commit_buffer(context_t *ctx);
 bool_t match_eof(context_t *ctx);
 bool_t match_eol(context_t *ctx);
 bool_t match_character(context_t *ctx, char ch);
 bool_t match_character_range(context_t *ctx, char min, char max);
 bool_t match_character_set(context_t *ctx, const char *chs);
 bool_t match_character_any(context_t *ctx);
 bool_t match_string(context_t *ctx, const char *str);
 bool_t match_blank(context_t *ctx);
 bool_t match_section_line_(context_t *ctx, const char *head);
 bool_t match_section_line_continuable_(context_t *ctx, const char *head);
 bool_t match_section_block_(context_t *ctx, const char *left, const char *right, const char *name);
 bool_t match_quotation_(context_t *ctx, const char *left, const char *right, const char *name);
 bool_t match_directive_c(context_t *ctx);
 bool_t match_comment(context_t *ctx);
 bool_t match_comment_c(context_t *ctx);
 bool_t match_comment_cxx(context_t *ctx);
 bool_t match_quotation_single(context_t *ctx);
 bool_t match_quotation_double(context_t *ctx);
 bool_t match_character_class(context_t *ctx);
 bool_t match_spaces(context_t *ctx);
 bool_t match_number(context_t *ctx);
 bool_t match_identifier(context_t *ctx);
 bool_t match_code_block(context_t *ctx);
 bool_t match_footer_start(context_t *ctx);
 node_t *parse_primary(context_t *ctx, node_t *rule);
 node_t *parse_term(context_t *ctx, node_t *rule);
 node_t *parse_sequence(context_t *ctx, node_t *rule);
 node_t *parse_expression(context_t *ctx, node_t *rule);
 node_t *parse_rule(context_t *ctx);
 const char *get_value_type(context_t *ctx);
 const char *get_auxil_type(context_t *ctx);
 const char *get_prefix(context_t *ctx);
 void dump_options(context_t *ctx);
const char * escape_file_path(const char *fname);
 bool_t parse_directive_include_(context_t *ctx, const char *name, FILE *output1, FILE *output2);
 bool_t parse_directive_string_(context_t *ctx, const char *name, char **output, string_flag_t mode);
 bool_t parse(context_t *ctx);
 code_reach_t generate_matching_string_code(generate_t *gen, const char *value, int onfail, int indent, bool_t bare);
 code_reach_t generate_matching_charclass_code(generate_t *gen, const char *value, int onfail, int indent, bool_t bare);
 code_reach_t generate_quantifying_code(generate_t *gen, const node_t *expr, int min, int max, int onfail, int indent, bool_t bare);
 code_reach_t generate_predicating_code(generate_t *gen, const node_t *expr, bool_t neg, int onfail, int indent, bool_t bare);
 code_reach_t generate_sequential_code(generate_t *gen, const node_array_t *nodes, int onfail, int indent, bool_t bare);
 code_reach_t generate_alternative_code(generate_t *gen, const node_array_t *nodes, int onfail, int indent, bool_t bare);
 code_reach_t generate_capturing_code(generate_t *gen, const node_t *expr, int index, int onfail, int indent, bool_t bare);
 code_reach_t generate_expanding_code(generate_t *gen, int index, int onfail, int indent, bool_t bare);
code_reach_t generate_thunking_action_code(
    generate_t *gen, int index, const node_const_array_t *vars, const node_const_array_t *capts, bool_t error, int onfail, int indent, bool_t bare
);
code_reach_t generate_thunking_error_code(
    generate_t *gen, const node_t *expr, int index, const node_const_array_t *vars, const node_const_array_t *capts, int onfail, int indent, bool_t bare
);
 code_reach_t generate_code(generate_t *gen, const node_t *node, int onfail, int indent, bool_t bare);
 bool_t generate(context_t *ctx);
 void print_version(FILE *output);
 void print_usage(FILE *output);

#endif