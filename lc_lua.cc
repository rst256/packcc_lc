%prefix "lc"
%value "int"
%auxil "parser_t*"

%header { 
	char * strdup(const char *);
	#include <string.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <getopt.h>


typedef int item_t;
inline size_t half_div_search(item_t t[], size_t size, item_t ch);
inline size_t half_div_search2(item_t t[], size_t size, item_t ch);
size_t half_div_search(item_t t[], size_t size, item_t ch){
	size_t begin=0, end=size;

	while (begin < end) {
		size_t mid = (begin + end) / 2;
		item_t c = t[mid];
		if ( c < ch ){
			begin = mid + 1;
		}else if ( c > ch ){
			if(mid==0 || t[mid-1] < ch) return mid;
			end = mid;
		}else {
			return mid+1;
		}
	}

	return 0;
}

size_t half_div_search2(item_t t[], size_t size, item_t ch){
	size_t begin=0, end=size;
	size_t mid = size * ch / t[end-1];

	while (begin < end) {
		item_t c = t[mid];
		if ( c < ch ){
			begin = mid + 1;
		}else if ( c > ch ){
			if(mid==0 || t[mid-1] < ch) return mid;
			end = mid;
		}else {
			return mid+1;
		}
		mid = (begin + end) / 2;
	}

	return 0;
}

#define table_size(t) (sizeof(t)/sizeof((t)[0]))

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




int get_line(parser_t* p, int pos){ 
	if(p->pos < pos) return 0;
	for(int i = 0; i<p->lines_count; i++) if(p->lines[i] > pos) return i+1;
	return p->lines_count+1; 
}

/*
int get_line2(parser_t* p, int ch){ 
	if(p->pos < ch) return 0;
	size_t begin=0, end=p->lines_count;

	while (begin < end) {
		size_t mid = (begin + end) / 2;
		item_t c = p->lines[mid];
		if ( c < ch ){
			begin = mid + 1;
		}else if ( c > ch ){
			if(mid==0 || p->lines[mid-1] < ch) return mid+1;
			end = mid;
		}else {
			return mid+2;
		}
	}
	return 0;
}


int get_line3(parser_t* p, int pos){ 
	if(p->pos < pos) return 0;
	return half_div_search(p->lines, p->lines_count, pos)+1;
}
*/

int get_col(parser_t* p, int pos){ 
	if(p->pos < pos) return 0;
	int line = half_div_search2(p->lines, p->lines_count, pos);
	return ( line  ? pos - p->lines[line-1] : pos ) + 1; 
}




#define CMPT_IDENT_TNAME "parser_ident"

static int createCmptIdent(lua_State *L, ident_t* id){
	ident_t** obj = lua_newuserdata(L, sizeof(ident_t*));
	if (obj == NULL) 
		luaL_error(L, "can't allocate memory for userdata '%s'", CMPT_IDENT_TNAME);
	*obj = id; 
	luaL_setmetatable(L, CMPT_IDENT_TNAME);
	return 1;
}

static int CmptIdent___tostring(lua_State *L){
	ident_t** id = luaL_checkudata(L, 1, CMPT_IDENT_TNAME);
	if (!id || !(*id)) 
		luaL_error(L, "userdata '%s' object has been freed", CMPT_IDENT_TNAME);
	lua_pushstring(L, ident_name(*id));
	return 1;
}






#define CMPT_SYMBOL_TNAME "parser_symbol"

static int createCmptSymbol(lua_State *L, symbol_t* id){
	if(!id){
		lua_pushnil(L);
		return 1;
	}
	symbol_t** obj = lua_newuserdata(L, sizeof(symbol_t*));
	if (obj == NULL) 
		luaL_error(L, "can't allocate memory for userdata '%s'", CMPT_SYMBOL_TNAME);
	*obj = id; 
	luaL_setmetatable(L, CMPT_SYMBOL_TNAME);
	return 1;
}

static int CmptSymbol___tostring(lua_State *L){
	symbol_t** self = luaL_checkudata(L, 1, CMPT_SYMBOL_TNAME);
	if (!self || !(*self)) 
		luaL_error(L, "userdata '%s' object has been freed", CMPT_SYMBOL_TNAME);
	lua_pushstring(L, symbol_name(*self));
	return 1;
}

static int CmptSymbol___call(lua_State *L){
	symbol_t** self = luaL_checkudata(L, 1, CMPT_SYMBOL_TNAME);
	if (!self || !(*self)) 
		luaL_error(L, "userdata '%s' object has been freed", CMPT_SYMBOL_TNAME);
	lua_rawgeti(L, LUA_REGISTRYINDEX, symbol_data(*self));
	return 1;
}

static int CmptSymbol___index(lua_State *L){
	symbol_t** self = luaL_checkudata(L, 1, CMPT_SYMBOL_TNAME);
	if (!self || !(*self)) 
		luaL_error(L, "userdata '%s' object has been freed", CMPT_SYMBOL_TNAME);
	lua_rawgeti(L, LUA_REGISTRYINDEX, symbol_data(*self));
	lua_pushvalue(L, 2);
	lua_gettable(L, -2);
	return 1;
}

static int CmptSymbol___newindex(lua_State *L){
	symbol_t** self = luaL_checkudata(L, 1, CMPT_SYMBOL_TNAME);
	if (!self || !(*self)) 
		luaL_error(L, "userdata '%s' object has been freed", CMPT_SYMBOL_TNAME);
	lua_rawgeti(L, LUA_REGISTRYINDEX, symbol_data(*self));
	lua_pushvalue(L, 2);
	lua_pushvalue(L, 3);
	lua_settable(L, -3);
	return 0;
}

/*
#define LUA_USERDATA_PTR(TYPE, ) luaL_checkudata(L, i, CMPT_OBJECT_TNAME);
static parser_t**  check_auxil(lua_State *L, int i){
	parser_t** ts = luaL_checkudata(L, i, CMPT_OBJECT_TNAME);
	if (ts == NULL || *ts == NULL ) 
		luaL_error(L, "userdata '%s' object has been freed", CMPT_OBJECT_TNAME);
	return ts;
}
*/

#define CMPT_OBJECT_TNAME "parser_auxil"

static parser_t**  check_auxil(lua_State *L, int i){
	parser_t** ts = luaL_checkudata(L, i, CMPT_OBJECT_TNAME);
	if (ts == NULL || *ts == NULL ) 
		luaL_error(L, "userdata '%s' object has been freed", CMPT_OBJECT_TNAME);
	return ts;
}
/*
static scope_t* check_scope(lua_State *L, int i){
	if(luaL_testudata(L, i, CMPT_OBJECT_TNAME)){
		return *check_auxil(L, i)->scope;
	}else{

	}
	scope_t** ts = luaL_checkudata(L, i, CMPT_SCOPE_TNAME);
	if (ts == NULL || *ts == NULL ) 
		luaL_error(L, "userdata '%s' object has been freed", CMPT_OBJECT_TNAME);
	return *ts;
}
*/

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
/*
#include <time.h>
#define be_loop for(int i=0, ps=0; i<1000000; i++, ps=ps<p->lines_count ? ps+1 : 0)
static int CmptObject_get_line_test(lua_State *L){
	parser_t* p = *check_auxil(L, 1);
	time_t t0, t;
	t0 = clock();
	int l;
	be_loop l = get_line(p, 22);
	t = clock();
	printf("get1 pos 22: %ld [%d]\n", t-t0, l);
	t0 = clock();
	be_loop l = get_line2(p, 22);
	t = clock();
	printf("get2 pos 22: %ld [%d]\n", t-t0, l);
	t0 = clock();
	be_loop l = get_line(p, 220);
	t = clock();
	printf("get1 pos 220: %ld [%d]\n", t-t0, l);
	t0 = clock();
	be_loop l = get_line2(p, 220);
	t = clock();
	printf("get2 pos 220: %ld [%d]\n", t-t0, l);

	t0 = clock();
	be_loop l = get_line(p, ps);
	t = clock();
	printf("get pos ~: %ld [%d]\n", t-t0, l);
	t0 = clock();
	be_loop l = get_line2(p, ps);
	t = clock();
	printf("get2 pos ~: %ld [%d]\n", t-t0, l);

	return 0;
}
*/

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
	//lua_pushlightuserdata(L, ident_add(p->idents, luaL_checkstring(L, 2)));
	createCmptIdent(L, ident_add(p->idents, luaL_checkstring(L, 2)));
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
	symbol_t* sym = luaL_testudata(L, 2, CMPT_SYMBOL_TNAME) ? 
		*(symbol_t**)luaL_checkudata(L, 2, CMPT_SYMBOL_TNAME) : 
			scope_find(p->scope, ( lua_type(L, 2)==LUA_TUSERDATA ? 
				*(ident_t**)lua_touserdata(L, 2) : 
				ident_add(p->idents, luaL_checkstring(L, 2)) 
			));
	if(sym) 
		createCmptSymbol(L, sym);
		// lua_rawgeti(L, LUA_REGISTRYINDEX, symbol_data(sym)); 
	else 
		lua_pushnil(L);
	return 1;
}

static int CmptObject_scope_add(lua_State *L){
	parser_t* p = *check_auxil(L, 1);
	if(lua_isnoneornil(L, 3)) lua_newtable(L);
	symbol_t* sym = scope_define(p->scope, 
		( lua_type(L, 2)==LUA_TUSERDATA ? 
			*(ident_t**)lua_touserdata(L, 2) : ident_add(p->idents, luaL_checkstring(L, 2)) ),
		luaL_ref(L, LUA_REGISTRYINDEX)
	);
	if(sym) 
		createCmptSymbol(L, sym);
	else 
		lua_pushnil(L);
	// lua_pushboolean(L, sym!=NULL);
	return 1;
}

static int scope_next_fn(lua_State *L){
	scope_iter_t iter = lua_touserdata(L, 1);
	if(scope_iter_end(iter)){
		lua_pushnil(L);
		scope_iter_delete(iter);		
		free(iter);
		return 1;
	}
	createCmptSymbol(L, scope_iter_value(iter));
	lua_pushlightuserdata(L, scope_iter_key(iter));
	scope_iter_next(iter);
	return 2;
}

static int CmptObject_scope_pairs(lua_State *L){
	parser_t* p = *check_auxil(L, 1);
	scope_iter_t iter = scope_iter_new(p->scope, NULL);
	lua_pushcfunction(L, scope_next_fn);
	lua_pushlightuserdata(L, iter);
	return 2;
}

static int CmptObject_scope_first(lua_State *L){
	parser_t* p = *check_auxil(L, 1);
	createCmptSymbol(L, scope_first(p->scope));
	return 1;
}

static int _scope_next(lua_State *L){
	symbol_t** self = luaL_checkudata(L, 1, CMPT_SYMBOL_TNAME);
	if (!self || !(*self)) 
		luaL_error(L, "userdata '%s' object has been freed", CMPT_SYMBOL_TNAME);
	createCmptSymbol(L, scope_next(*self));
	return 1;
}


/*

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
	symbol_t* sym = luaL_testudata(L, 2, CMPT_SYMBOL_TNAME) ? 
		*(symbol_t**)luaL_checkudata(L, 2, CMPT_SYMBOL_TNAME) : 
			scope_find(p->scope, ( lua_type(L, 2)==LUA_TUSERDATA ? 
				*(ident_t**)lua_touserdata(L, 2) : 
				ident_add(p->idents, luaL_checkstring(L, 2)) 
			));
	if(sym) 
		createCmptSymbol(L, sym);
		// lua_rawgeti(L, LUA_REGISTRYINDEX, symbol_data(sym)); 
	else 
		lua_pushnil(L);
	return 1;
}

static int CmptObject_scope_add(lua_State *L){
	parser_t* p = *check_auxil(L, 1);
	if(lua_isnoneornil(L, 3)) lua_newtable(L);
	symbol_t* sym = scope_define(p->scope, 
		( lua_type(L, 2)==LUA_TUSERDATA ? 
			*(ident_t**)lua_touserdata(L, 2) : ident_add(p->idents, luaL_checkstring(L, 2)) ),
		luaL_ref(L, LUA_REGISTRYINDEX)
	);
	if(sym) 
		createCmptSymbol(L, sym);
	else 
		lua_pushnil(L);
	// lua_pushboolean(L, sym!=NULL);
	return 1;
}

static int scope_next_fn(lua_State *L){
	scope_iter_t iter = lua_touserdata(L, 1);
	if(scope_iter_end(iter)){
		lua_pushnil(L);
		scope_iter_delete(iter);		
		free(iter);
		return 1;
	}
	createCmptSymbol(L, scope_iter_value(iter));
	lua_pushlightuserdata(L, scope_iter_key(iter));
	scope_iter_next(iter);
	return 2;
}

static int CmptObject_scope_pairs(lua_State *L){
	parser_t* p = *check_auxil(L, 1);
	scope_iter_t iter = scope_iter_new(p->scope, NULL);
	lua_pushcfunction(L, scope_next_fn);
	lua_pushlightuserdata(L, iter);
	return 2;
}

static int CmptObject_scope_first(lua_State *L){
	parser_t* p = *check_auxil(L, 1);
	createCmptSymbol(L, scope_first(p->scope));
	return 1;
}

static int _scope_next(lua_State *L){
	symbol_t** self = luaL_checkudata(L, 1, CMPT_SYMBOL_TNAME);
	if (!self || !(*self)) 
		luaL_error(L, "userdata '%s' object has been freed", CMPT_SYMBOL_TNAME);
	createCmptSymbol(L, scope_next(*self));
	return 1;
}
*/
	



static int CmptObject___gc(lua_State *L){
	//parser_t** obj = checkCmptObject(L, 1);
	//luaL_unref(L, LUA_REGISTRYINDEX, (*obj)->ctx);
	return 0;
}

static int CmptObject___tostring(lua_State *L){
	lua_pushstring(L, CMPT_OBJECT_TNAME);
	return 1;
}


static int Cmpt_parse_file(lua_State *L){
		const char * f = strdup(luaL_checkstring(L, 1));
		FILE * fd = fopen(f, "r");
		if(!fd) luaL_error(L, "can't open input file: `%s`\n", f);

			lua_newtable(L);	
			
			parser_t ps = (parser_t){ 
				L, fd, f, 0, 0, 0, luaL_ref(L, LUA_REGISTRYINDEX), 
				scope_new(NULL), new_idents()
			};



	createCmptObject(L, &(ps));
	lua_setglobal(L, "AUXIL");

		const char *include_file = "C:\\lib\\packcc\\lc_lua_h.lua";
			if(include_file){ 
				printf ("include file: %s\n", include_file); 
				if(luaL_dofile(L, include_file))
					luaL_error(L, "C:\\lib\\packcc\\%s\n", luaL_checkstring(L, -1));
			}
			if(luaL_dofile(L, "C:\\lib\\packcc\\lc_lua.lua"))
				luaL_error(L, luaL_checkstring(L, -1));

		lc_context_t *ctx = lc_create(&(ps));
		int ref;
		lc_parse(ctx, &ref);
			lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		lc_destroy(ctx);

	return 1;
}

	#define  OUT(...) fprintf(output, __VA_ARGS__)

}



########$ statement
stats <-  
	{ return block(auxil:clone()) } 
	( e:stat { table.insert(self, e) --print(auxil:pos(e.start_pos), e) } )+

stat <- ( 
		e:typeid/ e:while_loop / e:repeat_loop / e:for_loop 
		/ e:continue_cmd / e:break_cmd / e:ret_stat 
		/ e:define_func / e:if_then / e:block / e:assign 
		/ e:unop_stat / e:define / e:call _ 
	) { e.start_pos=_0s e.end_pos=_0e return e }


%rule typeid { printf("typeid\n"); } 


block <-
	'do' { auxil:scope_sub() return block(auxil:clone(), 'do', 'end') } _ ( e:stat { table.insert(self, e) } )+ _ 'end' _ { auxil:scope_up() } _


_block <-
	{ auxil:scope_sub() return block(auxil:clone()) } _ ( e:stat { table.insert(self, e) } )+ { auxil:scope_up() }

# 
# if_then <-
# 	'if' _ cond:expression _ 'then' _ th:_block _ ( 'else' _ el:_block _ )? 'end' _ 
# 		{ return if_then(cond, th, el) }

if_then <-
	'if' _ cond:expression _ 'then' _ th:_block _ 
	# { return asn_tmpl("if $cond then $th $el end", "if_then", { cond=cond, th=th }) }
	# (elif:elseif_list { self.elif = elif } )? _ 
	( 'else' _ el:_block _)? 'end' _ 
# {return asn_tmpl("if $cond then $th else $el end", "if_then", { cond=cond, th=th, el=el })}
{ return if_then(cond, th, el) }

# elseif_list <-
# 	'elseif' _ cf:expression _ 'then' _ thf:_block _ 
# 		{ return asn_list({ 
# 				asn_tmpl("elseif $c then $th", "elseif_block", { c=cf, th=thf })
# 			}, '\n', 'elseif_list')
# 		}
# 	( 'elseif' _ cn:expression _ 'then' _ thn:_block _ 
# 		{ table.insert(self, asn_tmpl("elseif $c then $th", "elseif_block", { c=cn, th=thn })) }
# 	)*

for_loop <-
	'for' _ { auxil:scope_sub() } loop_vars:define_args _ 'in' _ iter:expression _ 'do' _ body:_block _ 'end' _ 
		{ 
			auxil:scope_up()
			return asn_tmpl("for $loop_vars in $iter do $body end", 'for_loop', {
				loop_vars=loop_vars, iter=iter, body=body
			}) 
		}


while_loop <-
	'while' _ while_cond:expression _ 'do' _ body:_block _ 'end' _ 
		{ 
			return asn_tmpl("while $while_cond do $body end", 'while_loop', {
				while_cond=while_cond, body=body
			}) 
		}


repeat_loop <-
	'repeat' _ body:_block _ 'until' _ until_cond:expression _ 
		{ 
			return asn_tmpl("repeat $body until $until_cond", 'repeat_loop', {
				until_cond=until_cond, body=body
			}) 
		}


assign <-  
	var:_lvalue _ '=' _ 
		value:expression ~{ print('error expr') } _ { 
self = assign(var, value) 
return self }


define <-  
	e:type_expr _ ':' { return define(e) } _  
		< ident > _ ( '=' _ fv:expression ~{ print('error expr') } _ )? { self:add($1, fv) }
		( ',' _ < ident > _ ( '=' _ nv:expression ~{ print('error expr') } _ )? { self:add($2, nv) } )*



define_func <- 
	'function' _ rt:type_func_args _ ':' _ < ident > _ '(' _  { AUXIL:scope_sub() }
		fn_args:define_args? _ ')' _ body:_block? _ 'end' _   
			{ 
				local scope=AUXIL:clone() 
				auxil:scope_up() 
				return define_func{ 
					fn=$1, args=fn_args, ret_type=rt, 
					body=body, scope=scope 
				}
			}
	/ 'function' _ < ident > _ '(' _  { AUXIL:scope_sub() }
		fn_args:define_args? _ ')' _ body:_block? _ 'end' _   		
			{ 
				local scope=AUXIL:clone() 
				auxil:scope_up() 
				return define_func{ 
					fn=$2, args=fn_args,
					body=body, scope=scope 
				}
			}



define_args <-
 f:define_arg { return asn_list({ f }, ', ', 'define_args') }
	_ ( ',' _ n:define_arg { table.insert(self, n) } _ )* 

define_arg <-
	t:type_expr _ ':' _ < ident > { return define(t):add($1) }


ret_stat <-
	'return' { return asn_list({}, ', ', 'ret_stat', 'return ') } _ 
		( first:expression { table.insert(self, vars.first) } )? 
		_ ( ',' _ next:expression { table.insert(self, vars.next) } _ )* 


break_cmd <- 
	'break' _ { return asn_simple('break', 'break_cmd') }


continue_cmd <- 
	'continue' _ { return asn_simple('continue', 'continue_cmd') }





########$ expression 

expression <- 
	ee:assign { return ee }
	/ e:logic                   { return e }


logic <- 	
	larg:logic _ < '&&' / '||' / '^^' > _ rarg:bitwise   { return binop(larg, $1, rarg); }
	/ e:bitwise                  { return e }


bitwise <- 	
	larg:bitwise _ < '&' / '|' / '^' > _ rarg:compare   { return binop(larg, $1, rarg); }
	/ e:compare                  { return e }


compare <- 	
	larg:compare _ < '<=' / '>=' / '<' / '>' / '==' / '!=' > _ rarg:term   { return binop(larg, $1, rarg); }
	/ e:term                  { return e }


term <- 
	l:term _ < '+' / '-' > _ r:factor { return binop(vars.l, $1, vars.r); }
	/ e:factor                { return e }


factor <- 
	larg:factor _ < '*' / '/' / '%' > _ rarg:unop  { return binop(larg, $1, rarg) }
	/ e:unop                { return e }


unop <-
	< '$' / '@' / '--' / '-' / '++' / '!' > _ Uarg:unop { return unop($1, Uarg) }
	/ e:primary   { return e }



unop_stat <-
	< '--' / '++' > _ Uarg:unop { return unop($1, Uarg) }



primary <- 
	( '.' [0-9]+ ( 'e' [-+]? [0-9]+ )?   )      { return literal($0) }
	/ ( '0x' < [0-9a-fA-F]+ >  )       { return literal($0) }     
	/ [0-9]+ ( '.' [0-9]+ )? ( 'e' [-+]? [0-9]+ )? { return literal($0) }  
	/  ( [0-9]+ )             { return literal($0) }
	/ '"' < string > '"'  		{ return literal($0) }
	/ e:call 			{ return e }
	/ e:lvalue   
		{ return assert(auxil:scope_find(e), '\n'..AUXIL:pos(_0e)..tostring(e)..' undefined') }   
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


# idents_list <-
# 	< ident > { return asn_list({ $1 }, ', ', 'idents_list') }
# 	_ ( ',' _ < ident > { table.insert(self, $2) } _ )* 


_lvalue <- 
	obj:_lvalue _ '.' _ < ident > {  }
	/ obj:_lvalue _ '[' _ e:expression _ ']' {  }
	/ ident  { 
		return assert(auxil:scope_find($0), 
			'\n'..AUXIL:pos(_0e)..tostring($0)..' undefined') }
	/ '@' _ obj:_lvalue { return unop('@', obj)  }


lvalue <- 
	obj:lvalue _ '.' _ < ident > {  }
	/ obj:lvalue _ '[' _ e:expression _ ']' {  }
	/ ident  { 
return auxil:get_ident($0) }
	/ '@' _ obj:lvalue { return unop('@', obj)  }


type_expr <-
	e:type_expr _ 'const'  { e.qualifier='const' return e }
	/ e:type_expr _ '*' { return te.pointer(e) }
	/ e:type_expr _ '[' _ dim:expression _ ']' { return te.array(e, dim) }
	/ e:type_primary  { return e }

type_primary <-
	'const' _ e:base_type_expr { e.qualifier='const' return e }
	/ e:base_type_expr { return e }


base_type_expr <-
	e:type_func { return e }
	/ ident  { 
				local sym = AUXIL:scope_find($0)
				if sym then assert(sym.is_type)
					return te.primary(sym)
				end
				return te.primary($0) 
			}
	/ '(' _ e:type_expr _ ')' { return te.primary(e) }
	
type_func <-  
	'function' _ ( rt:type_func_args _ ':' _ )? '(' _ fn_args:type_func_args? _ ')'   
		{ return te.ftype(rt or false, fn_args or false) }


type_func_args <-
 	f:type_expr { return asn_list({ f }, ', ', 'type_func_args') }
	_ ( ',' _ n:type_expr { table.insert(self, n) } _ )* 




# access_modifiers <-
# 	'public'
# 	/ 'private'  
# 	/ 'protected'



_      <- [ \t\r\n]*





%%
/*
typedef int (*F)(void);
typedef char (*F1)(void);
typedef int (*F2)(char*);
F fg(int v){ return ({ int foo (void){ return v; }; &foo; }); }
F1 iter(char *s){printf("%s\n", s++);  return ({ char *ss=strdup(s), **p=&ss; char foo1 (void){char c=**p;if(c)(*p)+=1; return c; }; &foo1; }); }
static char ss[]="1234567890\0";

#define foreach(KEY, ITER) for(typeof((ITER)()) KEY; KEY=(ITER)();)
#define foreach2(KEY, ITER) for(char KEY; (ITER)(&(KEY));)
*/

void luainit_lc_lua(lua_State *L) {
   static luaL_Reg LuaLexer__fn[] = newluaL_Reg(CmptObject_,
   	clone, pos, get_line, get_col, get_file, get_ctx, get_ident, get_level,
		scope_sub, scope_up, scope_find, scope_add, 
		scope_pairs, scope_first
   );
	static luaL_Reg LuaLexer__mt[]= newluaL_Reg(CmptObject_,
		__tostring, __gc
	);
	LuaM_userdata_register_class(L, CMPT_OBJECT_TNAME, LuaLexer__fn, LuaLexer__mt);


   static luaL_Reg LuaIdent__fn[] = { NULL, NULL };
	static luaL_Reg LuaIdent__mt[]= newluaL_Reg(CmptIdent_,
		__tostring
	);
	LuaM_userdata_register_class(L, CMPT_IDENT_TNAME, LuaIdent__fn, LuaIdent__mt);


	static luaL_Reg LuaSymbol__mt[]= newluaL_Reg(CmptSymbol_,
		__tostring, __call, __index, __newindex
	);
	LuaM_userdata_register_proxy(L, CMPT_SYMBOL_TNAME, LuaSymbol__mt);


}

#define DLL_EXPORT __declspec(dllexport)

DLL_EXPORT LUALIB_API int luaopen_lc_lua1(lua_State *L) {
	luainit_lc_lua(L);
	static luaL_Reg LuaLexer__lib[]={
		{"parse_file", Cmpt_parse_file },
		{"scope_next", _scope_next },
		{NULL, NULL}
	};
	luaL_newlib(L, LuaLexer__lib);
	return 1;
}

int main(int argc, char** argv) {
//int (*f)(void) = ({ int foo (void){ return 4; }; &foo; });
//printf("%d\n", f());
//printf("\nforeach\n");
//F1 f1=({ char *s=ss; char foo1 (void){char c=*s;if(c)s++; return c; }; &foo1; });//iter(ss);
// foreach(c, f1) printf("%c ", c);
//
//printf("\nforeach2\n");
//F2 f2=({ char *s=ss; int foo1 (char *c){ *c=*s;if(*c)s++; return *c; }; &foo1; });//iter(ss);
// foreach2(c, f2) printf("%c ", c);
//printf("\n");
//
//f=fg(666);
//printf("%d\n", f());
//return 0;
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


	luainit_lc_lua(L);

	createCmptObject(L, &(ps[psi-1]));
	lua_setglobal(L, "AUXIL");

			if(include_file){ 
				printf ("include file: %s\n", include_file); 
				if(luaL_dofile(L, include_file)){
					fprintf(stderr, "C:\\lib\\packcc\\%s\n", luaL_checkstring(L, -1));
					exit(0);
				}
			}
			if(luaL_dofile(L, "C:\\lib\\packcc\\lc_lua.lua")){
				fprintf(stderr, luaL_checkstring(L, -1));
				exit(0);
			}

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