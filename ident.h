char * strdup(const char *);
#include <string.h>

#ifndef _IDENT_H_
#define _IDENT_H_

#include "uthash.h"


struct ident;
typedef struct ident ident_t;

struct ident_priv;
typedef int ident_priv_t;

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


ident_iter_t ident_first(idents_t *obj);
ident_iter_t ident_next(ident_iter_t i);

size_t ident_count(ident_t * i);
void ident_inccount(ident_t * i);
void ident_deccount(ident_t * i);

const char* ident_name(ident_t * i);
size_t ident_id(ident_t * i);


idents_t* new_idents();

ident_t * ident_add(idents_t *hash, const char* name);

ident_t * ident_add_src(idents_t *hash, const char * name, size_t str_len);

void ident_del(idents_t *hash, ident_t * i);

ident_t * ident_find_src(idents_t *hash, const char * name, size_t str_len);

ident_t * ident_find(idents_t *hash, const char* name);

size_t ident_clear(idents_t *hash);

void ident_free(idents_t **hash);


#endif // _IDENT_H_