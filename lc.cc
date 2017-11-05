%prefix "lc"
%value "ast_node_t"
%auxil "parser_t*"

%header { 
	char * strdup(const char *);
	#include <string.h>

	#include "ast.h"

}


%source {
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

}



########$ statement


statements <- 
	( _ statement _ ';'    )+

statement <- 
	block 
	/ callstat { printf("callstat: %s\n", $0); } 
	/ call { printf("call: %s\n", $0); } 
	/ assign 
	/ define_var_stat 


callstat <-
	< callstat > _ < ident / call > _ < statement > 
	/ < call > _ < statement >
	

block <-
	'{' _ statement _ ( ';' _ statement _ )* ';'? _ '}'  { printf("block: %s: %d %d\n", $0, $0s, $0e); }


assign <-  
	< ident > _ '=' _ < expression >




########$$ statement.declaration 


define_var_stat <- 
	te:type_expr _ ':'  _ ( assign / < IDENT > )

########$ expression 


expression <- 
	e:term 


term <- 
	larg:term _ < '+' / '-' > _ rarg:factor 
	/ e:factor                { $$ = e; }


factor <- 	
	larg:factor _ '*' _ rarg:unary  { $$.number = larg.number * rarg.number; }
	/ larg:factor _ '/'  _ rarg:unary   { $$.number = larg.number / rarg.number; } 
	/ e:unary                  { $$ = e; }


unary <- 
	'!' _ e:unary 
	/ '-' _ e:unary 
	/ '(' _ < tt:type_expr > _ ')' _ e:primary     
	/ e:primary     { $$ = e; }


primary <- 
	< [0-9]+ >               { $$.number = atoi($1); }
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


IDENT <- ident ~ { ERROR("expected ident\n"); } 

			

										
########$ type expression


type_expr <- 
	'@' type_expression
	/ e:type_expr _ < 'const' / 'static' / 'extern' / 'register'  >? _ '*'  
	/  e:type_expr   _ < '[' _ d:expression? _ ']' ~ { ERROR("expected `]`\n"); } >
	/ attr:type_attr _ e:type_expr ~ { ERROR("expected type_expr\n"); }   
	/ 'struct' _ e:ident _  fields
	/ 'struct' _ fields
	/ 'struct' _ e:ident ~ { ERROR("expected ident\n"); }
	/  e:ident  _ '<'  _ ( type_expr / expression ) _ ( ',' _ (type_expr / expression)  _ )* '>'
	/  e:ident    


type_expression <- 
	'{' _ < ( (type_expression / [^{}]+ ) _ )+ > '}' ~ { ERROR("expected `}`: %s\n", $0); } 
		#{ printf("type_expression: `%s`\n", $1); }


fields <- 
	'{' _ ( te:type_expr _ ':' ~ { ERROR("expected `:`\n"); } _ < IDENT >  _ ';' _ )+ '}'


type_attr <- 
	'const' / 'static' / 'extern' / 'register' 


_      <- ( [ \t\r] / EOL )*

EOL <- '\n' { line++; line_begin_pos=$0e; } 





%%
int main(int argc, const char* argv[]) {
	parser_t p = { argc>1 ? fopen(argv[1], "r") : stdin, argc>1 ? argv[1] : "stdin" };

    lc_context_t *ctx = lc_create(&p);
    while (lc_parse(ctx, NULL));
    lc_destroy(ctx);
    return 0;
}