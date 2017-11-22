/*!
files={}
files["scope.h"]={
	struct={ scope={}, symbol={}}
}
 */
 

#include <stdint.h>
#include <stdarg.h> 
#include "uthash.h"

#ifndef SYMDB_H
#define SYMDB_H
#include "ident.h"

typedef char * string;


struct symbol;
typedef struct symbol symbol_t;


struct scope;
typedef struct scope scope_t;

struct scope_priv;
typedef struct asn * scope_priv_t;

struct scope_iter;
typedef struct scope_iter * scope_iter_t;


struct scope_iter {
	struct scope *start;
	struct scope *scope;
	symbol_t *symbol;
	int readonly:1;
};


typedef int (*scope_iter_cb_t)(const symbol_t *s, const scope_t *scope);	
typedef void (*ident_data_free_cb_t)(const ident_priv_t data);	


scope_t* new_scope(scope_t *up);
scope_t* scope_new(scope_priv_t data);
scope_t* scope_child(scope_t *parent, scope_priv_t data);
scope_t* scope_parent(scope_t *s);

scope_priv_t scope_data(scope_t *scope);
void scope_setdata(scope_t *scope, scope_priv_t data);

int scope_ischild(scope_t *parent, scope_t *scope);

symbol_t* scope_define(scope_t *scope, ident_t *id, ident_priv_t data);

symbol_t* scope_find(scope_t *scope, ident_t *id);
symbol_t* scope_finde(scope_t *scope, ident_t *id, scope_t **defsc);
symbol_t* scope_findup(scope_t *scope, ident_t *id);
symbol_t *scope_search(scope_t *scope, ident_t *id, scope_t *before);

void scope_del(scope_t *scope, symbol_t * sym);
void scope_remi(scope_t *scope, ident_t * id);
void scope_undef(scope_t *scope, ident_t *ident);

symbol_t* scope_first(scope_t *scope);
symbol_t* scope_next(symbol_t *ss);

scope_iter_t scope_iter_new(scope_t *scope, scope_iter_t iter);
scope_iter_t scope_iter_clone(scope_iter_t from, scope_iter_t to);

void scope_iter_next(scope_iter_t ss);
void scope_iter_delete(scope_iter_t ss);
int scope_iter_end(scope_iter_t ss);
symbol_t* scope_iter_value(scope_iter_t i);
scope_t * scope_iter_key(scope_iter_t i);

void scope_foreach(scope_t *scope, scope_iter_cb_t cb);
void scope_clear(scope_t *scope, ident_data_free_cb_t free_cb);
void scope_free(scope_t **scope, ident_data_free_cb_t free_cb);

const char* symbol_name(const symbol_t * sym);
const ident_priv_t symbol_data(const symbol_t * sym);
const scope_t * symbol_scope(const symbol_t * sym);
void symbol_setdata(symbol_t * sym, const ident_priv_t data);

#endif

