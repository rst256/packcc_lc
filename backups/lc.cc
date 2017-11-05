%prefix "lc"
%value "ast_node_t"
%auxil "parser_t*"

%header { 
		const char * file_name;
	char * strdup(const char *);
	#include <string.h>

	#include "ast.h"
#define FACTOR(op, larg, rarg) op=='*' ? 		larg.number * rarg.number : larg.number / rarg.number
//#FACTOR()  

}
%source {
	int col = 1, line = 1;
	int pcc_getchar(parser_t* p){ 
		int c = fgetc(p->input); 
		if(c=='\n'){ line++; col=1; }else{ col++;} 
		return c; 
	}

    #define PCC_GETCHAR(auxil) pcc_getchar(auxil)
    #define PCC_BUFFERSIZE 1024

	#define  _ERROR(F, L, C, ...) { fprintf(stderr, "%s:%d:%d: ", F, L, C); fprintf(stderr, __VA_ARGS__); exit(0); }
	#define  ERROR(...) _ERROR(auxil->file_name, line, col, __VA_ARGS__)

}

statement <- _ '=' _ e:expression _ ';' { if(is_error) printf("error\n"); else printf("answer=%f\n", (e.number)); is_error = 0; }
				/ _ < ident > _ '=' _ e:expression  _ ';'  { 
	printf("`%s`=%f\n", $1, (e.number)); 
	int i=0;
	for (; vars[i].name; i++); 
	vars[i].name = strdup($1); 
	vars[i].value = (e.number);
}
				/  define_assign_var_stat 
				/ _

expression <- e:term { $$ = e; if($$.species=='+' || $$.species=='-' || $$.species=='*' || $$.species=='/') 
	printf("binop: %f %c %f = %f\n", $$.larg->number, $$.species, $$.rarg->number, $$.number); }

term <- larg::term _ < '+' / '-' > _ rarg::factor { 
																									$$.species = *$1; 
																									$$.number = larg.number + ($$.species=='-' ? -1 : 1) * rarg.number; 
																							}
      / e:factor                { $$ = e; }

factor <- larg::factor _ < '*' / '/' > _ rarg::unary { 
																								$$.species = *$1; 
																								$$.number = FACTOR($$.species,	larg ,rarg)  ; 
																								}
        / e:unary                  { $$ = e; }

unary <- '!' _ e:unary { $$.number = !(e.number); }
       / '-' _ e:unary { $$.number = -(e.number); }
       / '(' _ < tt:type_expr > _ ')' _ e:primary     { $$ = e; printf("cast: %s\n", $1); }
       / e:primary _ '('  _ expression? _ ( ',' _ expression _ )* ')'    { $$ = e; }
       / e:primary     { $$ = e; }

%source {
	int is_error = 0;
	static struct { 
		const char * name;
		double value;
	} vars[20] = {
		{ "pi", 3.1415926535897932384626433832795 },
		{ "e", 2.7182818284590452353602874713526 },
		{ NULL, 0 }
	};
}

primary <- < [0-9]+ >               { $$.number = atoi($1); }
			/ e:var   _ < '[' _ d:expression? _ ']' ~ "expected `]`" >   { $$ = e; }
			/ e:var     { $$ = e; }
         / '(' _ e:expression _ ')' { $$ = e; }

var <- <  [a-zA-Z_] [a-zA-Z0-9_]* >               { 	
	for (int i=0; vars[i].name; i++){		
		if(!strcmp(vars[i].name, $1)){ 
			$$.number = vars[i].value; return; 
		}
	} 
	printf("error var `%s` not defined\n", $1); 
	is_error = 1; 
}

ident <- [a-zA-Z_] [a-zA-Z0-9_]*

define_assign_var_stat <- _ < def:define_var >  _ '=' _ < val:expression > _ ';'  {printf("defassign_var_stat: %s = %s\n", $1, $2); } 
													/ define_var_stat
define_var_stat <- _ te:type_expr _ ':' ~ { ERROR("expected `:`\n"); } _ < ident > ~ { ERROR("expected ident\n"); }  _ ';'  { printf("define_var: %s\n", $1); } 

define_var <- te:type_expr _ ':' ~ { ERROR("expected `:`\n"); } _ < ident > ~ { ERROR("expected ident\n"); } 


type_expr <- e:type_expr _ < 'const' / 'static' / 'extern' / 'register'  >? _ '*'  
       /  e:type_expr   _ < '[' _ d:expression? _ ']' ~ { ERROR("expected `]`\n"); } >
       / attr:type_attr _ e:type_expr ~ { ERROR("expected type_expr\n"); }   
		 / 'struct' _ e:ident _ '{'  _  fields:define_var_stat+ { printf("fields: %p\n", &fields);} _ '}' ~ { ERROR("expected `}`\n"); } 
		 / 'struct' _ '{'  _  define_var_stat+ _ '}' ~ { ERROR("expected `}`\n"); } 
		 / 'struct' _ e:ident ~ { ERROR("expected ident\n"); }
       /  e:ident  _ '<'  _ ( type_expr / expression ) _ ( ',' _ (type_expr / expression)  _ )* '>'
       /  e:ident    

type_attr <- 'const' / 'static' / 'extern' / 'register' 

_      <- ( [ \t\r] / EOL )*
EOL <- '\n' 




%%
int main(int argc, const char* argv[]) {
	parser_t p = { argc>1 ? fopen(argv[1], "r") : stdin, argc>1 ? argv[1] : "stdin" };
file_name = p.file_name;
    lc_context_t *ctx = lc_create(&p);
    while (lc_parse(ctx, NULL));
    lc_destroy(ctx);
// getchar();
    return 0;
}