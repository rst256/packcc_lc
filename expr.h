/* A packrat parser generated by PackCC 1.2.0 */

#ifndef PCC_INCLUDED__EXPR_H
#define PCC_INCLUDED__EXPR_H

        char * strdup(const char *);
        #include <string.h>
        #include <stdio.h>
        #include <stdlib.h>
        #include <getopt.h>
        #include <malloc.h> 

//	include "ast.h"

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
        int lines[2048];
} parser_t;

#include "uthash.h"


struct ident;
typedef struct ident ident_t;

struct ident_priv;
typedef struct ident_priv * ident_priv_t;

struct idents;
typedef struct idents idents_t;

typedef ident_t * ident_iter_t;

struct ident{
        UT_hash_handle hh;
        char* name;
        size_t id;
        int count;
        unsigned int is_keyword:1;
};


struct idents{
        ident_t * idhash;
        size_t last_id;
};



ident_iter_t ident_first(idents_t *obj){ return obj ? obj->idhash : NULL; }
ident_iter_t ident_next(ident_iter_t i){ return i ? i->hh.next : NULL; }

size_t ident_count(ident_t * i){ return	i->count; }
void ident_inccount(ident_t * i){ i->count++; }
void ident_deccount(ident_t * i){ i->count--; }

const char* ident_name(ident_t * i){ return	i->name; }
size_t ident_id(ident_t * i){ return	i->id; }

static idents_t* Idents;

idents_t* new_idents(){
        idents_t *ids = (idents_t*)malloc(sizeof(idents_t));
        ids->idhash = NULL;
        ids->last_id = 0;
        return ids;
}

ident_t * ident_add(idents_t *hash, const char* name){
        ident_t * s;
        HASH_FIND_STR( hash->idhash, name, s );
        if(s) return s;
  s = (ident_t*)calloc(1, sizeof(ident_t));
  s->name = strdup(name); 
  hash->last_id++;
  s->id = hash->last_id;
  s->count = 0;
  HASH_ADD_KEYPTR(hh, hash->idhash, s->name, strlen(s->name), s);
        return s;
}

ident_t * ident_add_src(idents_t *hash, const char * name, size_t str_len){
        ident_t * s;
        HASH_FIND_STR( hash->idhash, name, s );
        if(s) return s;
  s = (ident_t*)malloc(sizeof(ident_t));
  s->name = memcpy(malloc(str_len+1), name, str_len);
        s->name[str_len] = 0;
  hash->last_id++;
  s->id = hash->last_id;
  s->count = 0;
  HASH_ADD_KEYPTR(hh, hash->idhash, s->name, strlen(s->name), s);
        return s;
}


void ident_del(idents_t *hash, ident_t * i){
        i->count--;
        if(i->count<=0) {
                free(i->name);
                HASH_DEL(hash->idhash, i);
                free(i);
        }
}

ident_t * ident_find_src(idents_t *hash, const char * name, size_t str_len){
        ident_t * s;
        char buff[str_len+1];
        memcpy(buff, name, str_len);
        buff[str_len] = 0;
        HASH_FIND_STR( hash->idhash, buff, s );
        return s;
}

// ident_t * ident_match(idents_t *hash, Source *src){
// 	const char * s = src->s;
// 	source_foreach(cc, src, charclass_id(cc) & charclass_ident_following);
// 	return ident_find_src(hash, s, src->s-s);
// }

ident_t * ident_find(idents_t *hash, const char* name){
        ident_t * s;
        HASH_FIND_STR( hash->idhash, name, s );
        return s;
}

size_t ident_clear(idents_t *hash){
        ident_t *s, *tmp;
        if(!hash) return 0;
        size_t len=0;
        HASH_ITER(hh, hash->idhash, s, tmp) {
                if(s->count<=0){
                        HASH_DEL(hash->idhash, s);
                        free(s->name);
                        free(s);
                }else{
                        len++;
                }
        }
        if(len==0) hash->idhash = NULL;
        return len;
}

void ident_free(idents_t **hash){
        if(ident_clear(*hash)==0)	*hash = NULL;
}


enum asn_type {
        expr_integer = 1,
        expr_binop, expr_ident, expr_call, stat_assign, expr_field, expr_index, expr_string, expr_float, 
};

typedef unsigned int parser_pos_t;

struct asn_shared {
        int type:28, is_statement:1;
        union {
                struct position_tag;
                //struct position_tag begin_pos;
        };
        //struct position_tag end_pos;
        parser_pos_t begin_pos;
        parser_pos_t end_pos;

};

struct asn_expr_primary {
        double number;
};

struct asn_expr_float {
        double float_num;
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

struct asn_expr_string {
        const char* string;
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


                //struct asn_expr_unop;
        };
} asn_t;

typedef struct asn_list {
        struct asn;
        struct asn_list* next;
} asn_list_t;




#ifdef __cplusplus
extern "C" {
#endif

typedef struct expr_context_tag expr_context_t;

expr_context_t *expr_create(parser_t*auxil);
int expr_parse(expr_context_t *ctx, asn_t**ret);
void expr_destroy(expr_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* PCC_INCLUDED__EXPR_H */
