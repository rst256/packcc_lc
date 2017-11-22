#include "ident.h"


ident_iter_t ident_first(idents_t *obj){ return obj ? obj->idhash : NULL; }
ident_iter_t ident_next(ident_iter_t i){ return i ? i->hh.next : NULL; }

size_t ident_count(ident_t * i){ return	i->count; }
void ident_inccount(ident_t * i){ i->count++; }
void ident_deccount(ident_t * i){ i->count--; }

const char* ident_name(ident_t * i){ return	i->name; }
size_t ident_id(ident_t * i){ return	i->id; }


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


