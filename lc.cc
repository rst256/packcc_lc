%prefix "lc"
%value "asn_t"
%auxil "parser_t*"

%header { 
	char * strdup(const char *);
	#include <string.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <getopt.h>

	#include "ast.h"

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

#define PCC_FREE(auxil, ptr)
}



########$ statement


statements <- 
	( _ statement _ ';'    )+
# { printf("stat: %s\n", $0); } 


statement <- 
	block 
	/ callstat { printf("callstat: %s\n", $0); } 
	# / ifstat { printf("ifstat: %s\n", $0); } 
	/ call #{ printf("call: %s\n", $0); } 
	/ assign 
	/ define_func { printf("define_func: %s\n", $0); } 
	/ decl_func { printf("decl_func: %s\n", $0); } 
	/ define_var_stat 


# ifstat <-
# 	'if' _ '(' _ expression _ ')' _ statement _ 'else'  _ statement
# 	/ 'if' _ '(' _ expression _ ')' _ statement
# 	/ 'if' _ '(' _ expression _ ')' 


callstat <-
	< callstat > _ < ident / __callstat / _callstat > _ < statement > 
	/ < _callstat / __callstat > _ < statement >
	/ _callstat  _ &';'

_callstat <-
	fn:lvalue _ '('  _ statement _ ( ';' _ ( statement / expression ) _ )* ')'    

__callstat <-
	funcstat _ '('  _ expression? _ ( ',' _ expression _ )* ')'   


block <-
	'{' _ statement _ ( ';' _ statement _ )* ';'? _ '}'  { printf("block: %s: %d %d\n", $0, $0s, $0e); }


assign <-  
	< ident > _ < [-+*/|&%^]? > '=' _ e:expression { 
if(e.type==2) printf("assign: %s = %d %c %d;\n", $1, e.larg->number, e.op, e.rarg->number);}




########$$ statement.declaration 


define_var_stat <- 
	te:_type_expr  _ ( assign / < ident > )


define_func <- 
	func_sign:decl_func _ func_body:statement


decl_func <- 
	te:_type_expr  _ < IDENT >  _ '('  _ arg_def? _ ( ',' _ arg_def _ )* ')'  


arg_def <-
	te:_type_expr  _ < IDENT >


########$ expression 


expression <- 
	e:logic                   { $$ = e; }


logic <- 	
	larg:logic _ < '&&' / '||' / '^^' > _ rarg:compare  
	/ e:compare                  { $$ = e; }


compare <- 	
	larg:compare _ < '<' / '>' / '<=' / '>=' / '==' / '!=' > _ rarg:term  
	/ e:term                  { $$ = e; }


term <- 
	(l:term _ < '+' / '-' > _ r:factor) { $$.type=2; $$.larg=&l; $$.rarg=&r; $$.op=$1[0]; }
	/ e:factor                { $$ = e; }


factor <- 	
	larg:factor _ '*' _ rarg:unary  { $$.number = larg.number * rarg.number; }
	/ larg:factor _ '/'  _ rarg:unary   { $$.number = larg.number / rarg.number; } 
	/ e:unary                  { $$ = e; }


unary <- 
	'!' _ e:unary 
	/ '-' _ e:unary 
	/ '--' _ e:unary 
	/ '++' _ e:unary 
	/ '*' _ e:unary 
	/ '&' _ e:unary 
	/ '~' _ e:unary 
	/ '(' _ < tt:type_expr > _ ')' _ e:primary     
	/ e:primary     { $$ = e; }


primary <- 
	< [0-9]+ >               { $$.number = atoi($1); $$.type = 1; }
	/ call     #{ $$ = e; }
	/ lvalue     #{ $$ = e; }
	/ '(' _ e:expression _ ')' { $$ = e; }


call <-
	fn:lvalue _ '('  _ expression? _ ( ',' _ expression _ )* ')'   


lvalue <-
	< lvalue > _ '.' _ < IDENT >   
	/ < lvalue > _ < '[' _ d:expression? _ ']'  >   
	/ < ident >     
	


ident <- [a-zA-Z_] [a-zA-Z0-9_]*


IDENT <- ident #~ { ERROR("expected ident\n"); } 

funcstat <-  
	[a-zA-Z_] [a-zA-Z0-9_]* { printf("funcstat: `%s`\n", $0);}


										
########$ type expression


type_expr <- 
	'@' type_expression
	/ e:type_expr _ < 'const' / 'static' / 'extern' / 'register'  >? _ '*'  
	/  e:type_expr   _ < '[' _ d:expression? _ ']' ~ { ERROR("expected `]`\n"); } >
	/ attr:type_attr _ e:type_expr ~ { ERROR("expected type_expr\n"); }   
	/ 'struct' _ e:ident _  fields
	/ 'struct' _ fields
	/ 'struct' _ e:ident ~ { ERROR("expected ident\n"); }
	/  e:ident  _ '<'  _ ( type_expr / term ) _ ( ',' _ (type_expr / term) { printf("param: `%s`\n", $0); } _ )* _ '>'
	/  e:ident    


type_expression <- 
	'{' _ < ( (type_expression / [^{}]+ ) _ )+ > '}' ~ { ERROR("expected `}`: %s\n", $0); } 
		#{ printf("type_expression: `%s`\n", $1); }


fields <- 
	'{' _ ( te:_type_expr _ < IDENT >  _ ';' _ )+ '}'
 # ~ { ERROR("expected `:`\n"); }


_type_expr <- 
	te:type_expr 


type_attr <- 
	'const' / 'static' / 'extern' / 'register' 


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

	OUT("int main(int argc, char** argv)\n{\n");

	for(int i = 0; i<psi; i++){
		printf ("translate file: %s\n", ps[i].file_name);
		lc_context_t *ctx = lc_create(&(ps[i]));
		while (lc_parse(ctx, NULL));
		lc_destroy(ctx);
	}

	OUT("\nreturn 0;\n}\n");




    return 0;
}