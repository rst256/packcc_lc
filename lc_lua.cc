%prefix "lc"
%value "int"
%auxil "parser_t*"

%header { 
	char * strdup(const char *);
	#include <string.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <getopt.h>

	#include "scope.h"

	typedef struct {
		lua_State* L;
		FILE* input;
		const char * file_name;
		int lines_count;
		int pos;
		int level;
		int ctx;
		scope_t* scope;
		idents_t* idents;
		int lines[2048];
	} parser_t;
	//# include "ast.h"
	int pcc_getchar(parser_t* p){ 
		int c;
		while( (c = fgetc(p->input))=='\r' );
		p->pos += 1;
		if(c=='\n'){ p->lines[p->lines_count++] = p->pos; }
		return c; 
	}

    #define PCC_GETCHAR(auxil) pcc_getchar(auxil)

}


%source {
	static FILE* output;

#define CMPT_OBJECT_TNAME "parser_auxil"
static parser_t**  check_auxil(lua_State *L, int i){
	parser_t** ts = luaL_checkudata(L, i, CMPT_OBJECT_TNAME);
	if (ts == NULL) luaL_error(L, "userdata '%s' object has been freed", CMPT_OBJECT_TNAME);
	return ts;
}

static int createCmptObject(lua_State *L, parser_t* parser){
	parser_t** obj = lua_newuserdata(L, sizeof(parser_t*));
	if (obj == NULL) 
		luaL_error(L, "can't allocate memory for userdata '%s' object", CMPT_OBJECT_TNAME);
	*obj = parser; 
	
	luaL_setmetatable(L, CMPT_OBJECT_TNAME);
	return 1;
}

static int CmptObject_clone(lua_State *L){
	parser_t* parser = *check_auxil(L, 1);
	parser_t** obj = lua_newuserdata(L, sizeof(parser_t*));
	if (obj == NULL) 
		luaL_error(L, "can't allocate memory for userdata '%s' object", CMPT_OBJECT_TNAME);
	*obj = malloc(sizeof(parser_t));
	**obj = *parser; 
	
	luaL_setmetatable(L, CMPT_OBJECT_TNAME);
	return 1;
}

int get_line(parser_t* p, int pos){ 
	if(p->pos < pos) return 0;
	for(int i = 0; i<p->lines_count; i++) if(p->lines[i] > pos) return i+1;
	return p->lines_count+1; 
}

int get_col(parser_t* p, int pos){ 
	if(p->pos < pos) return 0;
	int i;
	for(i = 0; i<p->lines_count; i++) if(p->lines[i] > pos) break;
	return ( i ? pos - p->lines[i-1] : pos ) + 1; 
}

static int CmptObject_pos(lua_State *L){
	parser_t* p = *check_auxil(L, 1);
	int pos = luaL_optinteger(L, 2, p->pos);
	lua_pushfstring(L, "%s:%d:%d: ", p->file_name, get_line(p, pos), get_col(p, pos));
	return 1;
}

static int CmptObject_get_line(lua_State *L){
	parser_t* p = *check_auxil(L, 1);
	lua_pushinteger(L, get_line(p, luaL_optinteger(L, 2, p->pos)));
	return 1;
}

static int CmptObject_get_col(lua_State *L){
	parser_t* p = *check_auxil(L, 1);
	lua_pushinteger(L, get_col(p, luaL_optinteger(L, 2, p->pos)));
	return 1;
}

static int CmptObject_get_level(lua_State *L){
	parser_t* p = *check_auxil(L, 1);
	lua_pushinteger(L, p->level);
	return 1;
}

static int CmptObject_get_file(lua_State *L){
	parser_t* p = *check_auxil(L, 1);
	lua_pushstring(L, p->file_name);
	return 1;
}

static int CmptObject_get_ctx(lua_State *L){
	parser_t* p = *check_auxil(L, 1);
	lua_rawgeti(L, LUA_REGISTRYINDEX, p->ctx);
	return 1;
}

static int CmptObject_get_ident(lua_State *L){
	parser_t* p = *check_auxil(L, 1);
	lua_pushlightuserdata(L, ident_add(p->idents, luaL_checkstring(L, 2)));
	return 1;
}

static int CmptObject_scope_sub(lua_State *L){
	parser_t* p = *check_auxil(L, 1);
	p->scope = scope_child(p->scope, NULL);
	p->level++;
	return 0;
}

static int CmptObject_scope_up(lua_State *L){
	parser_t* p = *check_auxil(L, 1);
	if( !(p->scope = scope_parent(p->scope)) ) luaL_error(L, "end of scope");
	p->level--;
	return 0;
}

static int CmptObject_scope_find(lua_State *L){
	parser_t* p = *check_auxil(L, 1);
	symbol_t* sym = scope_find(p->scope, 
		( lua_type(L, 2)==LUA_TLIGHTUSERDATA ? 
			lua_touserdata(L, 2) : ident_add(p->idents, luaL_checkstring(L, 2)) ));
	if(sym) 
		lua_rawgeti(L, LUA_REGISTRYINDEX, symbol_data(sym)); 
	else 
		lua_pushnil(L);
	return 1;
}

static int CmptObject_scope_add(lua_State *L){
	parser_t* p = *check_auxil(L, 1);
	symbol_t* sym = scope_define(p->scope, 
		( lua_type(L, 2)==LUA_TLIGHTUSERDATA ? 
			lua_touserdata(L, 2) : ident_add(p->idents, luaL_checkstring(L, 2)) ),
		luaL_ref(L, LUA_REGISTRYINDEX)
	);
	lua_pushboolean(L, sym!=NULL);
	return 1;
}

static int CmptObject___gc(lua_State *L){
	//parser_t** obj = checkCmptObject(L, 1);
	//luaL_unref(L, LUA_REGISTRYINDEX, (*obj)->ctx);
	return 0;
}

static int CmptObject___tostring(lua_State *L){
	lua_pushstring(L, CMPT_OBJECT_TNAME);
	return 1;
}



	#define  OUT(...) fprintf(output, __VA_ARGS__)

}



########$ statement
stats <-  
	( e:stat { print(auxil:pos(e.s), e) } )+

stat <-  
	( e:if_then / e:block / e:assign / e:define / e:call _ ) { e.s=_0s return e }

block <-
	'do' { auxil:scope_sub() return block(auxil:clone(), 'do', 'end') } _ ( e:stat { table.insert(self, e) } )+ _ 'end' _ { auxil:scope_up() } _


_block <-
	{ auxil:scope_sub() return block(auxil:clone()) } _ ( e:stat { table.insert(self, e) } )+ { auxil:scope_up() }


if_then <-
	'if' _ cond:expression _ 'then' _ th:_block _ ( 'else' _ el:_block _ )? 'end' _ 
		{ return if_then(cond, th, el) }


assign <-  
	var:lvalue _ '=' _ 
		value:expression ~{ print('error expr') } _ { 
self = assign(var, value) 
--if not auxil:scope_find(var) then 
--print(auxil:pos(_0s), "undefined: ", var) 
-- end
return self }

define <-  
	te:type_expr _ ':' _ < ident > _ ( '=' _ 
		value:expression ~{ print('error expr') } _ )? { 
self = define(te, $1, value) 
if not auxil:scope_add($1, te) then 
	print(auxil:pos(_0s), "redefine: ", $1, auxil:scope_find($1)) 
end
return self }


########$ expression 

expression <- 
	ee:assign { return ee }
	/ e:logic                   { return e }


logic <- 	
	l:logic _ < '&&' / '||' / '^^' > _ r:bitwise   { return binop(l, $1, r); }
	/ e:bitwise                  { return e }


bitwise <- 	
	l:bitwise _ < '&' / '|' / '^' > _ r:compare   { return binop(l, $1, r); }
	/ e:compare                  { return e }


compare <- 	
	l:compare _ < '<=' / '>=' / '<' / '>' / '==' / '!=' > _ r:term   { return binop(vars.l, $1, vars.r); }
	/ e:term                  { return e }


term <- 
	l:term _ < '+' / '-' > _ r:factor { return binop(vars.l, $1, vars.r); }
	/ e:factor                { return e }


factor <- 
	larg:factor _ < '*' / '/' / '%' > _ rarg:primary  { return binop(larg, $1, rarg) }
	/ e:unop                { return e }


unop <-
	< '$' / '@' / '--' / '-' / '++' / '!' > _ arg:unop { return unop($1, arg) }
	/ e:primary   { return e }


primary <- 
	( '.' [0-9]+ ( 'e' [-+]? [0-9]+ )?   )      { return tonumber($0) }
	/ ( '0x' < [0-9a-fA-F]+ >  )       { return tonumber($0) }     
	/ [0-9]+ ( '.' [0-9]+ )? ( 'e' [-+]? [0-9]+ )? { return tonumber($0) }  
	/  ( [0-9]+ )             { return tonumber($0) }
	/ '"' < string > '"'  		{ return ($0) }
	/ e:call 			{ return e }
	/ e:lvalue   { return e }   
	/ '(' _ e:expression _ ')' { return e }


string <-
	 string [^\"]
	/ '\"'
	/ [^\"]



ident <- !( 'do' / 'end' / 'else' ) ( [a-zA-Z_] [a-zA-Z0-9_]* )


call <-
	fn:lvalue _ '('    { return call(fn) }
	_ (first:expression {  self.args = { vars.first } })? 
	_ ( ',' _ next:expression { table.insert(self.args, vars.next) } _ )* 
	')'   



lvalue <- 
	obj:lvalue _ '.' _ < ident > {  }
	/ obj:lvalue _ '[' _ e:expression _ ']' {  }
	/ ident  { 
--print($0, auxil:get_ident($0), auxil:scope_find($0)) 
return $0--{ type='ident', name=$0 } }
	/ '@' _ obj:lvalue { return unop('@', obj)  }


type_expr <-
	te:type_expr _ '*' { return te_pointer(te) }
	/ te:type_expr _ 'const' _ '*' { return te_pointer(te, 'const') }
	/ te:type_expr _ '[' _ dim:expression _ ']' { return te_array(te, dim) }
	/ 'const' _ te:type_expr  { return te_primary(te, 'const') }
	/ ident  { return te_primary($0) }


# primary_types <-
# 	'struct' _ < ident > _ '{' _ '}' { return type_expr($1) }
# 	/ 'struct' _ '{' _ '}'  { return type_expr('struct') }
# 	/ 'struct' _ < ident >  { return type_expr($2) }
# 	/ ident  { return type_expr($0) }
# 	/ '(' _ te:type_expr _ ')' { return te }
# 	

_      <- [ \t\r\n]*






%%
int main(int argc, char** argv) {
	static int c, verbose_flag;
	output = stdin;
	const char* include_file = NULL;
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
          {"include",    required_argument, 0, 'i'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "o:i:",
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

        case 'i':
          printf ("option -i with value `%s'\n", optarg);
				include_file = strdup(optarg);
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
			lua_State*L = luaL_newstate();
			lua_newtable(L);	
			
			ps[psi++] = (parser_t){ 
				L, fd, f, 0, 0, 0, luaL_ref(L, LUA_REGISTRYINDEX), scope_new(NULL), new_idents()
			};
			luaL_openlibs(L);

   static luaL_Reg LuaLexer__fn[] = newluaL_Reg(CmptObject_,
   	clone, pos, get_line, get_col, get_file, get_ctx, get_ident, get_level,
		scope_sub, scope_up, scope_find, scope_add
   );
	static luaL_Reg LuaLexer__mt[]= newluaL_Reg(CmptObject_,
		__tostring, __gc
	);

	LuaM_userdata_register_class(L, CMPT_OBJECT_TNAME, LuaLexer__fn, LuaLexer__mt);
			if(include_file){ printf ("include file: %s\n", include_file); luaL_dofile(L, include_file);}
			luaL_dofile(L, "C:\\lib\\packcc\\lc_lua.lua");

		}
		
	}

	OUT("int main(int argc, char** argv)\n{\n");

	for(int i = 0; i<psi; i++){
		printf ("translate file: %s\n", ps[i].file_name);
		lc_context_t *ctx = lc_create(&(ps[i]));
		//# lc_loadcodes(ps[i].L);
		while (lc_parse(ctx, NULL));
		lc_destroy(ctx);
	}

	OUT("\nreturn 0;\n}\n");




    return 0;
}