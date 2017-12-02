
// #include "luam/lua5x.h"
#include "lc_lua.h"


int get_line(parser_t* p, int pos){ 
	if(p->pos < pos) return 0;
	for(int i = 0; i<p->lines_count; i++) if(p->lines[i] > pos) return i+1;
	return p->lines_count+1; 
}

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
	if( !lua_isnoneornil(L, 2)){
		symbol_setdata(*self, luaL_ref(L, LUA_REGISTRYINDEX));
	}
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


#define CMPT_OBJECT_TNAME "parser_auxil"

static parser_t**  check_auxil(lua_State *L, int i){
	parser_t** ts = luaL_checkudata(L, i, CMPT_OBJECT_TNAME);
	if (ts == NULL || *ts == NULL ) 
		luaL_error(L, "userdata '%s' object has been freed", CMPT_OBJECT_TNAME);
	return ts;
}

int createCmptObject(lua_State *L, parser_t* parser){
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

