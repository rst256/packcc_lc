%prefix "calc"
%value "double"

statement <- _ '=' _ e:expression _ EOL { if(is_error) printf("error\n"); else printf("answer=%f\n", e); is_error = 0; }
				/ _ < [a-z]+ > _ '=' _ e:expression _ EOL { if(is_error) printf("error\n"); else printf("`%s`=%f\n", $1, e); is_error = 0;
int i=0;for (; vars[i].name; i++); vars[i].name = strdup($1); vars[i].value = e;
}
           / ( !EOL . )* EOL      { printf("error\n"); }

expression <- e:term { $$ = e; }

term <- l:term _ '+' _ r:factor { $$ = l + r; }
      / l:term _ '-' _ r:factor { $$ = l - r; }
      / e:factor                { $$ = e; }

factor <- l:factor _ '*' _ r:unary { $$ = l * r; }
        / l:factor _ '/' _ r:unary { $$ = l / r; }
        / e:unary                  { $$ = e; }

unary <- '+' _ e:unary { $$ = +e; }
       / '-' _ e:unary { $$ = -e; }
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

primary <- < [0-9]+ >               { $$ = atoi($1); }
			/ e:var     { $$ = e; }
         / '(' _ e:expression _ ')' { $$ = e; }

var <- < [a-z]+ >               { 	for (int i=0; vars[i].name; i++){		if(!strcmp(vars[i].name, $1)){ $$ = vars[i].value; return; };	} printf("error var `%s` not defined\n", $1); is_error = 1; }

_      <- [ \t]*
EOL    <- '\n' / '\r\n' / '\r' / ';'

%%
int main() {
    calc_context_t *ctx = calc_create(NULL);
    while (calc_parse(ctx, NULL));
    calc_destroy(ctx);
    return 0;
}