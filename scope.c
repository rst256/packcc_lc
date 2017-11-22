

#include "scope.h"
#include <stdio.h>

// #define HASH(K, V) typedef struct symbol {	UT_hash_handle hh; K; V; } 
// 
// HASH(ident_t * ident, ident_priv_t data) symbol_t;
typedef struct symbol {
	UT_hash_handle hh; 
	ident_t * ident;
	ident_priv_t data;
	struct scope * scope;
} symbol_t;

struct scope {
	symbol_t *symdb;
	struct scope *up;
	struct scopes * childs;
	scope_priv_t data;
};

struct scopes {
	UT_hash_handle hh; 
	struct scope *scope;
};


scope_iter_t scope_iter_new(scope_t *scope, scope_iter_t iter){
	scope_iter_t i = iter ? iter : malloc(sizeof(struct scope_iter));
	i->start = scope;
	i->scope = scope;
	i->symbol = scope->symdb;
	return i;
}

scope_iter_t scope_iter_clone(scope_iter_t from, scope_iter_t to){
	scope_iter_t i = to ? to : malloc(sizeof(struct scope_iter));
	i->start = from->start;
	i->scope = from->scope;
	i->symbol = from->symbol;
	from->readonly = 1;
	i->readonly = from->readonly;
	return i;
}
void scope_iter_next(scope_iter_t i){
	if(i->symbol) i->symbol = i->symbol->hh.next;
	if(!i->symbol){
		i->scope = i->scope->up;
		if(i->scope) i->symbol = i->scope->symdb;
	}
	if(i->symbol)
		if(scope_search(i->start, i->symbol->ident, i->scope)) scope_iter_next(i);
}

void scope_iter_delete(scope_iter_t i){
	if(!i->symbol) return;
	if( i->readonly){
		scope_iter_next(i);
	}else{
		symbol_t* tmpi = i->symbol;
		scope_t * tmps = i->scope;
		scope_iter_next(i);
		scope_del(tmps, tmpi);
	}
}

int scope_iter_end(scope_iter_t i){ return i->symbol==NULL; }
symbol_t* scope_iter_value(scope_iter_t i){ return i->symbol; }
scope_t * scope_iter_key(scope_iter_t i){ return i->scope; }

#ifdef RELEASE
	#define NOT_NULL(X) 
#else
	#define NOT_NULL(X) if( !(X) ) {\
		printf("\nNull assertion: %s==NULL, in function %s, file \"%s\":%d\n\n", \
			#X, __FUNCTION__, __FILE__, __LINE__); exit(__LINE__);}
#endif


#define PROP_RO(OT, PT, PN) \
PT OT##_##PN(OT##_t *obj){ return obj->PN; }


#define PROP_RW(OT, PT, PN) \
PT OT##_get##PN(OT##_t *obj){ return obj->PN; }\
void OT##_set##PN(OT##_t *obj, PT value){	obj->PN = value; }

#define ITER_FN(OT, IT, HS) \
IT* OT##_first(OT##s_t *obj){ return obj ? obj->HS : NULL; }\
IT* OT##_next(IT * i){ return	i->hh.next; }


scope_t* new_scope(scope_t *up){
	scope_t *sc = calloc(1, sizeof(scope_t));
	// scope_t *sc = malloc(sizeof(scope_t));
	// sc->up = up;
	// sc->childs = NULL;
	// sc->data = NULL;
	// sc->symdb = NULL;
	if(up){
		sc->up = up;
		struct scopes *scs = malloc(sizeof(struct scopes));
		scs->scope = sc;
		HASH_ADD_PTR(up->childs, scope, scs);
	}
	return sc;
}

scope_t* scope_parent(scope_t *s){ return s->up; }

scope_t* scope_child(scope_t *parent, scope_priv_t data){
	// NOT_NULL(parent);
	scope_t *sc = calloc(1, sizeof(scope_t));
	// scope_t *sc = malloc(sizeof(scope_t));
	sc->up = parent;
	sc->data = data;
	// sc->childs = NULL;
	// sc->symdb = NULL;
	if(parent){
		struct scopes *scs = malloc(sizeof(struct scopes));
		scs->scope = sc;
		HASH_ADD_PTR(parent->childs, scope, scs);
	}
	return sc;
}

scope_t* scope_new(scope_priv_t data){
	// scope_t *sc = malloc(sizeof(scope_t));
	scope_t *sc = calloc(1, sizeof(scope_t));
	// sc->symdb = NULL;
	// sc->up = NULL;
	// sc->childs = NULL;
	sc->data = data;
	return sc;
}

scope_priv_t scope_data(scope_t *scope){ 
	return scope->data; 
}


symbol_t* scope_define(scope_t *scope, ident_t *id, ident_priv_t data){
	NOT_NULL(scope);
	symbol_t * ss;
	HASH_FIND_PTR(scope->symdb, &id, ss);
	if(ss) return NULL;
	ident_inccount(id);
  ss = malloc(sizeof(symbol_t));
  ss->ident = id; ss->data = data; ss->scope = scope; 
  HASH_ADD_PTR(scope->symdb, ident, ss);
	return ss;
}

int scope_ischild(scope_t *parent_scope, scope_t *child_scope){
	NOT_NULL(parent_scope); NOT_NULL(child_scope);
	while(child_scope)	
		if(parent_scope==child_scope->up) return 1; else child_scope = child_scope->up;
	return 0;
}

symbol_t *scope_find(scope_t *scope, ident_t *id){
	NOT_NULL(id); NOT_NULL(scope); 
	symbol_t * ss;
	scope_t *sc = scope;
	while(sc)	{
		HASH_FIND_PTR(sc->symdb, &id, ss);
		if(ss) return ss;
		sc = sc->up;
	}
	return NULL; 
}

symbol_t* scope_finde(scope_t *scope, ident_t *id, scope_t **defsc){
	NOT_NULL(id); NOT_NULL(scope); NOT_NULL(defsc); 
	symbol_t * ss;
	scope_t *sc = scope;
	while(sc)	{
		HASH_FIND_PTR(sc->symdb, &id, ss);
		if(ss){ *defsc = sc; return ss; }
		sc = sc->up;
	}
	*defsc = NULL;
	return NULL; 
}

// Поиск определения идента в области от scope до up_bound       
symbol_t *scope_search(scope_t *scope, ident_t *id, scope_t *up_bound){
	NOT_NULL(id); NOT_NULL(scope); NOT_NULL(up_bound); 
	symbol_t * ss;
	scope_t *sc = scope;
	while(sc && sc!=up_bound)	{
		HASH_FIND_PTR(sc->symdb, &id, ss);
		if(ss) return ss;
		sc = sc->up;
	}
	return NULL; 
}

/* Найти исходное определение. 
 Если ид ident был переопределен в области scope тогда будет возвращено 
 его 
 (т.е. ident имеет определение 
*/
symbol_t *scope_findup(scope_t *scope, ident_t *ident){
	NOT_NULL(scope); NOT_NULL(ident); 
	symbol_t * local_sym = scope->up ? scope_find(scope, ident) : NULL;
	if(!local_sym) return NULL;
	symbol_t * upper_sym = scope_find(scope->up, ident);
	return upper_sym!=local_sym ? upper_sym : NULL;
}

void scope_undef(scope_t *scope, ident_t *ident){
	NOT_NULL(ident); NOT_NULL(scope);
	symbol_t * sym = scope_find(scope, ident);
	if(!sym) return;
	ident_deccount(ident);
	HASH_DEL(scope->symdb, sym);
	free(sym);
}

void scope_del(scope_t *scope, symbol_t * ss){
	NOT_NULL(ss); NOT_NULL(scope);
	ident_deccount(ss->ident);
	HASH_DEL(scope->symdb, ss);
	free(ss);
}

void scope_clear(scope_t *scope, ident_data_free_cb_t free_cb){
	NOT_NULL(scope);
	symbol_t *s, *tmp;
	struct scopes *scs, *scs_tmp;
	HASH_ITER(hh, scope->childs, scs, scs_tmp) {		scope_clear(scs->scope, free_cb); 	}
	HASH_ITER(hh, scope->symdb, s, tmp) {		scope_del(scope, s);  }
	scope->symdb = NULL;
}

void scope_free(scope_t **scope, ident_data_free_cb_t free_cb){
	NOT_NULL(scope); NOT_NULL(*scope); 
	scope_clear(*scope, free_cb);
	free(*scope);
	*scope = NULL;
}
	
void scope_remi(scope_t *scope, ident_t * id){
	NOT_NULL(scope); NOT_NULL(id); 
	if(id){
		symbol_t * ss;
		ident_deccount(id);
		HASH_FIND_PTR(scope->symdb, &id, ss);
		if(ss) HASH_DEL(scope->symdb, ss);
	}
}

symbol_t* scope_first(scope_t *scope){
	NOT_NULL(scope);
	return scope->symdb;
}

symbol_t* scope_next(symbol_t *ss){
	NOT_NULL(ss);
	return ss->hh.next;
}

void scope_foreach(scope_t *scope, scope_iter_cb_t cb){
NOT_NULL(scope); NOT_NULL(scope);
	symbol_t * s;
	scope_t *sc = scope;
	while(sc){
		for(s=sc->symdb; s != NULL; s=s->hh.next){
			if(!scope_search(scope, s->ident, sc)) cb(s, sc);
		}
		sc = sc->up;
	}
}

const char* symbol_name(const symbol_t * sym){
	NOT_NULL(sym);
	return ident_name(sym->ident);
}

const scope_t * symbol_scope(const symbol_t * sym){
	NOT_NULL(sym);
	return sym->scope;
}

const ident_priv_t symbol_data(const symbol_t * sym){
	NOT_NULL(sym);
	return sym->data;
}

void symbol_setdata(symbol_t * sym, const ident_priv_t data){
	NOT_NULL(sym);
	sym->data = data;
}

