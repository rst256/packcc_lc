%prefix "expr"
%value "asn_t*"
%auxil "parser_t*"

%header { 
	char * strdup(const char *);
	#include <string.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <getopt.h>

//	include "ast.h"

typedef struct {
	FILE* input;
	const char * file_name;
	// struct {
	// 
	// } ;
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
	expr_primary = 1,
	expr_binop, expr_ident, expr_call, stat_assign, expr_field, expr_index, expr_string, expr_float, 
};

struct asn_shared {
        int type;
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



}


%source {
	static FILE* output;

	int col = 1, line = 1; static line_begin_pos=0;
	int pcc_getchar(parser_t* p){ 
		int c = fgetc(p->input); 
		return c; 
	}

    #define PCC_GETCHAR(auxil) pcc_getchar(auxil)
    #define PCC_BUFFERSIZE 1024

	#define  _ERROR(F, L, C, FN, ...) { fprintf(stderr, "%s:%d:%d: {%s} ", F, L, C, FN); fprintf(stderr, __VA_ARGS__); exit(0); }
	#define  ERROR(...) _ERROR(auxil->file_name, (_0s), (_0e), __FUNCTION__, __VA_ARGS__)

	#define  _WARN(F, L, C, ...){ fprintf(stderr, "%s:%d:%d: ", F, L, C); fprintf(stderr, __VA_ARGS__); }
	#define  WARN(...) _WARN(auxil->file_name, (_0s), (_0e), __VA_ARGS__)

	#define  OUT(...) fprintf(output, __VA_ARGS__)

	#define PCC_ERROR(auxil) pcc_error(auxil, __FILE__, __LINE__, __FUNCTION__)
	static void pcc_error(parser_t* p, const char *f, int l, const char *fn) {
		fprintf(stderr, "%s:%d: Syntax error in %s \n", f, l, fn);
		exit(1);
	}

    #define _NEW_ASN(TYPE, THIS) THIS = malloc(sizeof(asn_t)); THIS->type = TYPE;
    #define NEW_ASN(TYPE) _NEW_ASN(TYPE, __)

void print_asn(asn_t *n){
	if(!n) return;
	switch(n->type){
		case expr_primary:
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
		case stat_assign:
			print_asn(n->lvalue); printf(" = "); print_asn(n->rvalue); printf(";"); 
			break;
		default:
			printf("?[%d]?", n->type);
		break;
	}
}


}



########$ statement

statement <- 
	e:assign   { $$ = e; }
	/ _ c:call _ ';'    { $$ = c; }
	/ _ { $$ = NULL; }

assign <-  
	_ lv:lvalue _ < [-+*/|&%^]? > '=' _ rv:expression _ ';'  {  
NEW_ASN(stat_assign); 
$$->lvalue = lv;
$$->rvalue = rv;

//printf("assign: %s = ", $1);

//printf(";\n");
}




########$ expression 
%source {

asn_t * binop(asn_t * l, const char *op, asn_t * r){
	asn_t * __;
	if(l->type==expr_primary && r->type==expr_primary){
		NEW_ASN(expr_primary); 
		switch(op[0]){
		  case '-': __->number = l->number - r->number; break;
		  case '+': __->number = l->number + r->number; break;
		  case '/': __->number = l->number / r->number; break;
		  case '*': __->number = l->number * r->number; break;
		  default:
		    
		    break;
		}
	}else{
		NEW_ASN(expr_binop); 
		$$->larg=l; $$->rarg=r; $$->op=op[0]; 
	}
	return __;
}

}

expression <- 
	e:term  { $$ = e; }

term <- 
	l:term _ < '+' / '-' > _ r:factor { $$ = binop(l, $1, r); }
	/ e:factor                { $$ = e; }


factor <- 
	l:factor _ < '*' / '/' > _ r:primary { $$ = binop(l, $1, r); }
	/ e:primary                { $$ = e; }



primary <- 
	 '.' [0-9]+ ( 'e' [-+]? [0-9]+ )?               { NEW_ASN(expr_float); $$->float_num = atof($0); }
	/ < [0-9]+ ( '.' [0-9]+ )? ( 'e' [-+]? [0-9]+ )? >               { NEW_ASN(expr_float); $$->float_num = atof($0); }
	/ < [0-9]+ >               { NEW_ASN(expr_primary); $$->number = atoi($2); }
	/ '"' < string > '"'  			{ printf("string: `%s`\n", $3); NEW_ASN(expr_string); $$->string = strdup($3);}
	# / string 		{ $$ = e; }
	/ e:call 			{ $$ = e; }
	/ e:lvalue   { $$ = e; }   
	/ '(' _ e:expression _ ')' { $$ = e; }


string <-
	 string [^\"]
	/ '\"'
	/ [^\"]

call <-
	fn:lvalue _ '('    { 
			NEW_ASN(expr_call); 
			$$->func = fn; //$$->args = NULL; 
			$$->args_count = 0; $$->args = calloc(30, sizeof(asn_t *));
		}  
	_ (first:expression { $$->args[($$->args_count)++] = first;  })? 
	_ ( ( ',' _ next:expression  _ ) { $$->args[($$->args_count)++] = next; } )* ')'  


lvalue <- 
	obj:lvalue _ '.' _ < ident > { NEW_ASN(expr_field); $$->object = obj; $$->field = ident_add(Idents, $1); }
	/ obj:lvalue _ '[' _ e:expression _ ']' { NEW_ASN(expr_index); $$->array = obj; $$->index = e; }
	/ ident  { NEW_ASN(expr_ident); $$->ident = ident_add(Idents, $0); 
//strdup($0); 
}


ident <- [a-zA-Z_] [a-zA-Z0-9_]*




_      <- ( [ \t\r] / EOL )*

EOL <- '\n' { line++; line_begin_pos=$0e; } 





%%
int main(int argc, char** argv) {
	static int c, verbose_flag;
	output = stdin;

  while (1)    {
      static struct option long_options[] =
        {
          /* These options set a flag. */
          {"verbose", no_argument,       &verbose_flag, 1},
          {"brief",   no_argument,       &verbose_flag, 0},
          /* These options don’t set a flag.
             We distinguish them by their indices. */
          {"add",     no_argument,       0, 'a'},
          {"append",  no_argument,       0, 'b'},
          {"output",  required_argument, 0, 'o'},
          {"create",  required_argument, 0, 'c'},
          {"file",    required_argument, 0, 'f'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "o:",
                       long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
        {
        case 0:
          /* If this option set a flag, do nothing else now. */
          if (long_options[option_index].flag != 0)
            break;
          printf ("option %s", long_options[option_index].name);
          if (optarg)
            printf (" with arg %s", optarg);
          printf ("\n");
          break;

        case 'a':
          puts ("option -a\n");
          break;

        case 'b':
          puts ("option -b\n");
          break;

        case 'c':
          printf ("option -c with value `%s'\n", optarg);
          break;

        case 'o':
          output = fopen(optarg, "w+");
          break;

        case 'f':
          printf ("option -f with value `%s'\n", optarg);
          break;

        case '?':
          /* getopt_long already printed an error message. */
          break;

        default:
          abort ();
        }
    }

  /* Instead of reporting ‘--verbose’
     and ‘--brief’ as they are encountered,
     we report the final status resulting from them. */
  if (verbose_flag)
    puts ("verbose flag is set");

  /* Print any remaining command line arguments (not options). */
	//if (optind < argc)

	parser_t ps[20];
	int psi = 0;
	while (optind < argc && psi<20){
		const char * f = argv[optind++];
		FILE * fd = fopen(f, "r");
		if(!fd){
			fprintf(stderr, "can't open input file: `%s`\n", f);
		}else{
			//printf ("translate file: %s\n", f);
			ps[psi++] = (parser_t){ fd, f };
		}
		
	}

	Idents = new_idents();
	
	OUT("int main(int argc, char** argv)\n{\n");

	for(int i = 0; i<psi; i++){
		printf ("translate file: %s\n", ps[i].file_name);
		expr_context_t *ctx = expr_create(&(ps[i]));
		asn_t* n;
		while (expr_parse(ctx, &n)){ print_asn(n); printf("\n"); }
		expr_destroy(ctx);
	}

	OUT("\nreturn 0;\n}\n");




    return 0;
}