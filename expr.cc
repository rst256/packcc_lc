%prefix "expr"
%value "asn_t*"
%auxil "parser_t*"

%header { 
	

//	include "ast.h"




#include "ast.h"

static idents_t* Idents;





}


%source {
	static FILE* output;
//	int col = 1, line = 1; static line_begin_pos=0;
	int pcc_getchar(parser_t* p){ 
		int c;
		while( (c = fgetc(p->input))=='\r' );
		p->pos += 1;
		if(c=='\n'){ p->lines[p->lines_count++] = p->pos; }
		return c; 
	}

    #define PCC_GETCHAR(auxil) pcc_getchar(auxil)
    #define PCC_BUFFERSIZE 1024


	//#define DEBUG_ERROR_PREFIX( FN, LN, FL ) fprintf(stderr, "%s:%d: in `%s`:\n", FL, L, FN)

	//#define  _ERROR(F, L, C, FN, ...) { fprintf(stderr, "%s:%d:%d: {%s} ", F, L, C, FN); fprintf(stderr, __VA_ARGS__); exit(0); }
#define  _DEBUG_ERROR_PREFIX( AUX, S, E, C, FN, L, FL, ... ) \
	( \
		fprintf(stderr, "%s:%d:%d: Start ", AUX->file_name, get_line(AUX, S), get_col(AUX, S) ), \
		fprintf(stderr, __VA_ARGS__), \
		fprintf(stderr, "\n\t%s:%d:%d: Ends `%s`\n\t\t%s:%d: error in %s:\n", \
				AUX->file_name, get_line(AUX, E), get_col(AUX, E), C, \
				FL, L, FN \
		) \
	)
	#define  DEBUG_ERROR_PREFIX( AUX, S, E, C, FN, L, ... ) \
		_DEBUG_ERROR_PREFIX( AUX, S, E, C, FN, L, __VA_ARGS__, "Syntax error!")

//	#define  UpperFilter(Parent, ...) Parent ( (__VA_ARGS__ , __LINE__) ) 


	#define  _ERROR(AUX, PREF, ...) { DEBUG_ERROR_PREFIX(AUX, FL, L, FN); fprintf(stderr, __VA_ARGS__); exit(0); }
	#define  ERROR(...)  exit( ( DEBUG_ERROR_PREFIX( auxil, _0s, _0e,  _0, __FUNCTION__, __LINE__, __FILE__, __VA_ARGS__) , 0 ) ) 

	#define  EXPECT(...)   exit( ( DEBUG_ERROR_PREFIX( auxil, _0s, _0e,  _0, __FUNCTION__, __LINE__, __FILE__, #__VA_ARGS__ " expected"), 0 ) )

	#define  _WARN(F, L, C, ...){ fprintf(stderr, "%s:%d:%d: ", F, L, C); fprintf(stderr, __VA_ARGS__); }
	#define  WARN(...) _WARN(auxil->file_name, (_0s), (_0e), __VA_ARGS__)

	#define  OUT(...) fprintf(output, __VA_ARGS__)

	#define PCC_ERROR(auxil) pcc_error(auxil, __FILE__, __LINE__, __FUNCTION__)
	static void pcc_error(parser_t* p, const char *f, int l, const char *fn) {
		fprintf(stderr, "%s:%d: Syntax error in %s \n", f, l, fn);
		exit(1);
	}

	#define _NEW_ASN(TYPE, THIS, SRC, CTX) \
		THIS = malloc(sizeof(asn_t)); \
		THIS->type = TYPE; THIS->is_statement = 0; 
	#define NEW_ASN(TYPE) _NEW_ASN(TYPE, __, _0, auxil)


#define INIT_LIST(L) (L)->count = 0; \
			(L)->items = calloc(300, sizeof(*((L)->items))); 

#define INSERT_LIST(L, I) \
			(L)->items[((L)->count)++] = (I);


}



########$ statement

__statements <- 
	__statement+

statements <- { auxil->scope = scope_child(auxil->scope, NULL); }
	first:statement _ ( ';' _)? 
		{			
			NEW_ASN(block); 
			$$->count = 0; 
			//$$->scope = scope_child(auxil->scope, $$);
			//$$->scope = scope_new($$);
			$$->nodes = calloc(LC_MAX_BLOCK_ITEMS, sizeof(*($$->nodes)));
			$$->nodes[($$->count)++] = first;
		}  
	( next:statement _ ( ';' _)? { $$->nodes[($$->count)++] = next; } )*
		{
			auxil->scope = scope_parent(auxil->scope);
			if($$->count == 1){//fixme: 
				free($$->nodes); free($$); $$ = first;
			}else{
				$$->nodes= realloc($$->nodes, sizeof(asn_t *)*($$->count));
			}
		}





__statement <- 
 	e:statement  _
		{
				$$ = e; 
				printf("%s:%d:%d:", auxil->file_name, 
					get_line(auxil, $$->begin_pos), get_col(auxil, $$->begin_pos));
				print_asn(e); 
				printf("\n");
		} 
	/ ';' _





statement <- 
	 e:if_then    { $$ = e; $$->begin_pos = _0s; $$->end_pos = _0e; }
	/ e:if_then_else    { $$ = e; $$->begin_pos = _0s; $$->end_pos = _0e; }
	/e:assign  { $$ = e; $$->begin_pos = _0s; $$->end_pos = _0e; }
	/ c:call    { $$ = c; $$->begin_pos = _0s; $$->end_pos = _0e; }
	/ e:define_var    { $$ = e; $$->begin_pos = _0s; $$->end_pos = _0e; }


define_var <-
	te:type_expr _ ':' _ < ident >    { NEW_ASN(stat_define); $$->def_type = te; $$->def_ident = ident_add(Idents, $1);  $$->init_value = NULL; symbol_t* sym = scope_define(auxil->scope, $$->def_ident, NULL); if(!sym) printf("redefine: `%s`\n", $1);}
	( _ '=' _ e:expression { $$->init_value = e; } )?
	( _ ',' _ < ident > ( _ '=' _ e2:expression )? )*

assign <-  
	lv:lvalue _ < [-+*/|&%^] > '=' _ rv:expression  
		{ NEW_ASN(stat_assign); $$->lvalue = lv; $$->rvalue = rv; }
	/ lv:lvalue _ '=' _ rv:expression  
		{ NEW_ASN(stat_assign); $$->lvalue = lv; $$->rvalue = rv; }


if_then <-
	'if' _ cond:expression _ 'then' _ th:statements _ 'end'
		{ NEW_ASN(stat_if); $$->condition = cond; $$->then_block = th; }

if_then_else <-
	'if' _ cond:expression _ 'then' _ th:statements _ 'else' _ el:statements _ 'end'
		{ NEW_ASN(stat_if_then_else); 
$$->condition = cond; 
$$->then_block = th; 
$$->else_block = el; 
}



########$ expression 


expression <- 
	ee:assign { $$ = ee; $$->begin_pos = _0s; $$->end_pos = _0e; }
	/ e:logic                   { $$ = e; $$->begin_pos = _0s; $$->end_pos = _0e; }


logic <- 	
	l:logic _ < '&&' / '||' / '^^' > _ r:compare   ~{ EXPECT(rarg); } { $$ = binop(l, $1, r, auxil, $0); }
	/ e:compare                  { $$ = e; }


compare <- 	
	l:compare _ < '<' / '>' / '<=' / '>=' / '==' / '!=' > _ r:term   ~{ EXPECT(rarg); } { $$ = binop(l, $1, r, auxil, $0); }
	/ e:term                  { $$ = e; }


term <- 
	l:term _ < '+' / '-' > _ r:factor ~{ EXPECT(rarg); } { $$ = binop(l, $1, r, auxil, $0); }
	/ e:factor                { $$ = e; }


factor <- 
	l:factor _ < '*' / '/' > _ r:primary ~{ ERROR("`%s` rarg expected", $1); }  { $$ = binop(l, $1, r, auxil, $0); }
	/ e:primary                { $$ = e; }



primary <- 
	( '.' [0-9]+ ( 'e' [-+]? [0-9]+ )?   )            { NEW_ASN(expr_float); $$->float_num = atof($0); }
	/ ( '0x' < [0-9a-fA-F]+ >  )            
		{ 
			NEW_ASN(expr_integer); 
			char buff[128], c;
			const char *s = $1; int i, r, rr = 1;
			for(i = 0; (c = *s++); i++){
				if(c>='0' && c<='9') buff[i] = c - '0';
				else if(c>='a' && c<='f') buff[i] = 10 + c - 'a';
				else if(c>='A' && c<='F') buff[i] = 10 + c - 'A';
				//printf("%2d: %c %2d %2x\n", i, c, buff[i], buff[i]);
			} //i--;
			for(r = 0; --i >= 0;){ r += rr*buff[i]; rr *= 16; }
			$$->number = r; 
			// printf("hex: `%s` %x %d\n", $1, r, r); 
		}
	/ ([0-9]+ ( '.' [0-9]+ )? ( 'e' [-+]? [0-9]+ )?   )             { NEW_ASN(expr_float); $$->float_num = atof($0); }
	/  ( '-'? [0-9]+ )              { NEW_ASN(expr_integer); $$->number = atoi($0); }
	/ '"' < string > '"'  			
		{ /*printf("string: `%s`\n", $2); */
			NEW_ASN(expr_string); 
			$$->string = strdup($2);
		}
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
			$$->args_count = 0; $$->args = calloc(LC_MAX_FUNCTION_ARGS, sizeof(asn_t *));
		}  
	_ (first:expression { $$->args[($$->args_count)++] = first;  })? 
	_ ( !')' ( ','? _ next:expression ~{ EXPECT(function call argument); } _ ) { $$->args[($$->args_count)++] = next; } )* 
	')'   ~{ EXPECT(')'); }
		{
			if($$->args_count){
				$$->args = realloc($$->args, sizeof(asn_t *)*$$->args_count);
			}else{
				free($$->args); $$->args = NULL;
			}
		}


lvalue <- 
	obj:lvalue _ '.' _ < ident > { NEW_ASN(expr_field); $$->object = obj; $$->field = ident_add(Idents, $1); }
	/ obj:lvalue _ '[' _ e:expression _ ']' { NEW_ASN(expr_index); $$->array = obj; $$->index = e; }
	/ ident  { NEW_ASN(expr_ident); $$->ident = ident_add(Idents, $0); 
symbol_t* sym = scope_find(auxil->scope, $$->ident); if(sym) printf("defined: `%s` %p\n", $0, sym);
}


ident <- [a-zA-Z_] [a-zA-Z0-9_]*


IDENT <- ident #~ { ERROR("expected ident\n"); } 


########$ type expression


type_expr <- 
	 e:type_expr _ '*'  		
		{ 
			NEW_ASN(type_expr); $$->base_type = e; 
			$$->te_type = te_pointer; $$->is_const = 0; 
		}
	 / e:type_expr _ 'const' _ '*'  		
		{ 
			NEW_ASN(type_expr); $$->base_type = e; 
			$$->te_type = te_pointer; $$->is_const = 1; 
		}

	/  e:type_expr   _ < '[' _ d:expression? _ ']' ~ { ERROR("expected `]`\n"); } >
	/ 'const' _ e:type_expr ~ { ERROR("expected type_expr\n"); }   
		{ 
			NEW_ASN(type_expr); $$->base_type = e; 
			$$->te_type = te_primary; $$->is_const = 1; 
		}
	/ 'struct' _ e:ident _  fields
	/ 'struct' _ fields
	/ 'struct' _ e:ident ~ { ERROR("expected ident\n"); }
	/  e:ident  _ '<'  _ ( type_expr / term ) _ ( ',' _ (type_expr / term) { printf("param: `%s`\n", $0); } _ )* _ '>'
	/  e:ident    { NEW_ASN(expr_ident); $$->ident = ident_add(Idents, $0); }




fields <- 
	'{' _ ( te:type_expr _ < IDENT >  _ ';' _ )+ '}'
 # ~ { ERROR("expected `:`\n"); }



# type_attr <- 
# 	'const' / 'static' / 'extern' / 'register' 


_      <- [ \t\r\n]*






%%
    //static pcc_value_t null = NULL;


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
			ps[psi++] = (parser_t){ fd, f, (struct position_tag){1, 1}, (struct position_tag){1, 1}, 0, 0, scope_new(NULL) };
		}
		
	}

	Idents = new_idents();
	
	OUT("int main(int argc, char** argv)\n{\n");

	for(int i = 0; i<psi; i++){
		printf ("translate file: %s\n", ps[i].file_name);
		expr_context_t *ctx = expr_create(&(ps[i]));
		asn_t* n;expr_parse(ctx, &n);
/*		while (){ 
			printf("%s:%d:%d:", ps[i].file_name, ps[i].line, ps[i].col);
			print_asn(n); 
			printf("\n"); 
		}*/
		expr_destroy(ctx);
	}

	OUT("\nreturn 0;\n}\n");




    return 0;
}