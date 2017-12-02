/*
солипсизм не уверен, но вот на уровне вида, расы и тем более нации/социума они несомненно практически неразличимы.
можете мне поверить, может на физическом уровне они и имеют четкие различия, тот генокод, но сами по себе они практически неразличимы. да они и не стремятся по моему, про логины я даже не заикаюсь подавляющему большинству мы их сами "придумываем", но так они с трудом могут  генерируют или практически одинаковые  так еще и 
интересно было бы провозвестии эксперимент 


интересно было бы провозвестии эксперимент на способность человека различать людей (простите за тавтологию) не опираясь на биометрические параметры и взаимно известные факты.
фактически это немного измененный оригинальный тест тьюринга, тот где 


мне как то ближе теория что мы это информация, единство сознания просто необходимое условие для функционирования инстинкта самосохранения, например подобное расширения границ сознания может быть вполне штатной операцией для  активации  социального инстинкта самосохранения
у него как я понимаю есть строго определенные ситуации когда 
7201
398363699

*/
#include "luam/lua5x.h"
#include "packcc.h"
#include "utstring.h"

static lua_State*L;

bool_t is_line_first(context_t *ctx, int p) {
	for(int i = p - 1; i>0; i--) switch (ctx->buffer.buf[i]){
		case ' ':
		case '\t':
		case '\r':
			break;
		case '\n':
			return TRUE;
		default:
			return FALSE;
	}
	return FALSE;
}

bool_t match_directive(context_t *ctx) {
    int p = ctx->bufpos, q, ds, de, bs, be;
    int l = ctx->linenum;
    int m = ctx->bufpos - ctx->linepos;


	if (!is_line_first(ctx, p)) return FALSE;

	// while (match_character_set(ctx, " \t\r"));
	// if (!match_character(ctx, '\n')) goto EXCEPTION;
	// while (match_character_set(ctx, " \t\r"));
	if (!match_string(ctx, "//@")) goto EXCEPTION;
	while (match_character_set(ctx, " \t\r"));
	ds = ctx->bufpos;
	if (!match_identifier(ctx)) goto EXCEPTION;
	de = ctx->bufpos;
	while (match_character_set(ctx, " \t\r"));
	bs = ctx->bufpos;
	while (!match_eof(ctx)) {
		int p = ctx->bufpos;
		if (match_eol(ctx)) {
			if (ctx->buffer.buf[p - 1] != '\\') break;
		} else {
			match_character_any(ctx);
		}
	}
	be = ctx->bufpos;

	char* d = strndup_e(ctx->buffer.buf + ds, de - ds);
	char* b = strndup_e(ctx->buffer.buf + bs, be - bs);
	fprintf(ctx->hfile, "%s:%d:%d: ", ctx->iname, l + 1, m + 1);
	fprintf(ctx->hfile, "`%s` '%s'\n", d, b);
	free(d);
	free(b);



    return TRUE;

EXCEPTION:;
    ctx->bufpos = p;
    ctx->linenum = l;
    ctx->linepos = p - m;
    return match_section_line_continuable_(ctx, "#");
}

static int parse_macro_args(context_t *ctx) {
	static const char open_char = '(', close_char = ')', sep_char = ','; 
    int l = ctx->linenum;
    int m = ctx->bufpos - ctx->linepos;
    if (match_character(ctx, open_char)) {
		match_spaces(ctx);
    	int p = ctx->bufpos;
		if (match_character(ctx, close_char)) {
			return 0;
        }
        int d = 1, argcount = 1;
		luaL_Buffer b; luaL_buffinit(L, &b);
        for (;;) {
            if (match_eof(ctx)) {
                print_error("%s:%d:%d: Premature EOF in args list\n", ctx->iname, l + 1, m + 1);
                ctx->errnum++;
                break;
            }
			int pp = ctx->bufpos; 
            if (
                match_directive_c(ctx) ||
                match_comment_c(ctx) ||
                match_comment_cxx(ctx) ||
                match_quotation_single(ctx) ||
                match_quotation_double(ctx)
            ) {
				luaL_addlstring(&b, ctx->buffer.buf + pp, ctx->bufpos - pp);
				continue;
			}
            if (match_character(ctx, open_char)) {
                d++; luaL_addchar(&b, open_char);
            }
            else if (match_character(ctx, close_char)) {
                d--; 
                if (d == 0) break;luaL_addchar(&b, close_char);
            }
            else if (match_character(ctx, sep_char)) {
				if(d==1){
					//lua_pushlstring(L, ctx->buffer.buf + p, ctx->bufpos - p - 1);
					luaL_pushresult(&b); luaL_buffinit(L, &b);
					argcount++;
	                p = ctx->bufpos;
				}else
					 luaL_addchar(&b, sep_char);
            }
            else {
                if (!match_eol(ctx)) {
                    if (match_character(ctx, '$')) {
                        ctx->buffer.buf[ctx->bufpos - 1] = '_';
                    }
                    else {
                        match_character_any(ctx);
                    }
                }
				luaL_addchar(&b, ctx->buffer.buf[ctx->bufpos - 1]);
            }
        }
		luaL_pushresult(&b);
//         *args++ = strndup_e(ctx->buffer.buf + p, ctx->bufpos - p - 1);
// 		*args = NULL;
        return argcount;
    }
    return 0;
}

static bool_t parse_macro_block(context_t *ctx, luaL_Buffer* b) {
	static const char open_char = '{', close_char = '}', sep_char = ','; 
    int l = ctx->linenum;
    int m = ctx->bufpos - ctx->linepos;
    if (match_character(ctx, open_char)) {
    	int p = ctx->bufpos;
		luaL_addchar(b, open_char);
        int d = 1;
        for (;;) {
            if (match_eof(ctx)) {
                print_error("%s:%d:%d: Premature EOF in args list\n", ctx->iname, l + 1, m + 1);
                ctx->errnum++;
                break;
            }
			int pp = ctx->bufpos; 
            if (
                match_directive_c(ctx) ||
                match_comment_c(ctx) ||
                match_comment_cxx(ctx) ||
                match_quotation_single(ctx) ||
                match_quotation_double(ctx)
            ) {
				luaL_addlstring(b, ctx->buffer.buf + pp, ctx->bufpos - pp);
				continue;
			}
            if (match_character(ctx, open_char)) {
                d++; luaL_addchar(b, open_char);
            }
            else if (match_character(ctx, close_char)) {
                d--; luaL_addchar(b, close_char);
                if (d == 0) break;
            }
            else {
                if (!match_eol(ctx)) {
                    if (match_character(ctx, '$')) {
                        ctx->buffer.buf[ctx->bufpos - 1] = '_';
                    }
                    else {
                        match_character_any(ctx);
                    }
                }
				luaL_addchar(b, ctx->buffer.buf[ctx->bufpos - 1]);
            }
        }
		// luaL_pushresult(&b);
//         *args++ = strndup_e(ctx->buffer.buf + p, ctx->bufpos - p - 1);
// 		*args = NULL;
        return TRUE;
    }
    return FALSE;
}
static bool_t match_macro(context_t *ctx) {
    int p = ctx->bufpos, ns, ne;
    int l = ctx->linenum;
    int m = ctx->bufpos - ctx->linepos;

    //match_spaces(ctx);
    if (!match_character(ctx, '@')) goto EXCEPTION;

    ns = ctx->bufpos;
    if (!match_identifier(ctx)) goto EXCEPTION;
    ne = ctx->bufpos;
    match_spaces(ctx);

//     const char* args[20];
//     if (!parse_macro_args(ctx, args, 20)){
// 		args[0] = NULL;
// 		int q = ctx->bufpos;
// 		if( match_quotation_double(ctx) || match_quotation_single(ctx) ){
// 			args[0] = strndup_e(ctx->buffer.buf + q + 1, ctx->bufpos - q - 2);
// 			args[1] = NULL;
// 		}
// 	}

   
	char* nm = strndup_e(ctx->buffer.buf + ns, ne - ns);


	fprintf(ctx->hfile, "%s:%d:%d: @%s\n", ctx->iname, l + 1, m + 1, nm);



if( lua_getglobal(L, nm) == LUA_TFUNCTION ){
	// lua_rawgeti(L, LUA_REGISTRYINDEX, (*__pcc_out));
	// createCmptObject(L, __pcc_ctx->auxil);
	// lua_newtable(L);
	// lua_pushstring(L, pcc_get_capture_string(__pcc_ctx, &__pcc_in->data.leaf.capt0));
	// lua_setfield(L, -2, "_0"); 
	// lua_pushinteger(L, __pcc_in->data.leaf.capt0.range.start);
	// lua_setfield(L, -2, "_0s"); 
	// lua_pushinteger(L, __pcc_in->data.leaf.capt0.range.end);
	// lua_setfield(L, -2, "_0e"); 
	// lua_newtable(L);
	// lua_rawgeti(L, LUA_REGISTRYINDEX, (*__pcc_in->data.leaf.values.buf[0]));
	// lua_setfield(L, -2, "e"); 
	int argcount = parse_macro_args(ctx);
	// for(argcount = 0; args[argcount]; argcount++)
	// 	lua_pushstring(L, args[argcount]);
	lua_call(L, argcount, 1);    
	if(!lua_isnoneornil(L, -1)) fputs(lua_tostring(L, -1), ctx->sfile);
}else{
	fprintf(stderr, "`%s` is not a function\n", nm);
}

	//fprintf(ctx->sfile, "%.*s", ctx->bufpos - ne, ctx->buffer.buf + ne);

	free(nm);
    return TRUE;

EXCEPTION:;
    ctx->bufpos = p;
    ctx->linenum = l;
    ctx->linepos = p - m;
    return FALSE;
}

bool_t match_other(context_t *ctx) {
    int p = ctx->bufpos, q, ds, de, bs, be;
    int l = ctx->linenum;
    int m = ctx->bufpos - ctx->linepos;
    if (
        match_macro(ctx)
	) return TRUE;

    if (
        match_directive_c(ctx) ||
        match_comment_c(ctx) ||
        match_comment_cxx(ctx) ||
        match_quotation_single(ctx) ||
        match_quotation_double(ctx)
    ) {
		fprintf(ctx->sfile, "%.*s", ctx->bufpos - p, ctx->buffer.buf + p);
	    return TRUE;
	}


// 
// EXCEPTION:;
//     ctx->bufpos = p;
//     ctx->linenum = l;
//     ctx->linepos = p - m;
    return FALSE;
}

static bool_t parse_args(context_t *ctx, const char** args, size_t args_size) {
	static const char open_char = '(', close_char = ')', sep_char = ','; 
	assert(args_size>0);
    int l = ctx->linenum;
    int m = ctx->bufpos - ctx->linepos;
    if (match_character(ctx, open_char)) {
		match_spaces(ctx);
    	int p = ctx->bufpos;
		if (match_character(ctx, close_char)) {
			*args = NULL;
			return TRUE;
        }
        int d = 1;
        for (;;) {
            if (match_eof(ctx)) {
                print_error("%s:%d:%d: Premature EOF in args list\n", ctx->iname, l + 1, m + 1);
                ctx->errnum++;
                break;
            }
            if (
                match_directive_c(ctx) ||
                match_comment_c(ctx) ||
                match_comment_cxx(ctx) ||
                match_quotation_single(ctx) ||
                match_quotation_double(ctx)
            ) continue;
            if (match_character(ctx, open_char)) {
                d++;
            }
            else if (match_character(ctx, close_char)) {
                d--;
                if (d == 0) break;
            }
            else if (match_character(ctx, sep_char) && d==1) {
                *args++ = strndup_e(ctx->buffer.buf + p, ctx->bufpos - p - 1);
                p = ctx->bufpos;
            }
            else {
                if (!match_eol(ctx)) {
                    if (match_character(ctx, '$')) {
                        ctx->buffer.buf[ctx->bufpos - 1] = '_';
                    }
                    else {
                        match_character_any(ctx);
                    }
                }
            }
        }
        *args++ = strndup_e(ctx->buffer.buf + p, ctx->bufpos - p - 1);
		*args = NULL;
        return TRUE;
    }
    return FALSE;
}

static bool_t parse_fields(context_t *ctx, const char** args, size_t args_size) {
	static const char open_char = '{', close_char = '}', sep_char = ';'; 
	assert(args_size>0);
    int l = ctx->linenum;
    int m = ctx->bufpos - ctx->linepos;
    if (match_character(ctx, open_char)) {
		match_spaces(ctx);
    	int p = ctx->bufpos;
		if (match_character(ctx, close_char)) {
			*args = NULL;
			return TRUE;
        }
        int d = 1;
        for (;;) {
            if (match_eof(ctx)) {
                print_error("%s:%d:%d: Premature EOF in args list\n", ctx->iname, l + 1, m + 1);
                ctx->errnum++;
                break;
            }
            if (
                match_directive_c(ctx) ||
                match_comment_c(ctx) ||
                match_comment_cxx(ctx) ||
                match_quotation_single(ctx) ||
                match_quotation_double(ctx)
            ) continue;
            if (match_character(ctx, open_char)) {
                d++;
            }
            else if (match_character(ctx, close_char)) {
                d--;
                if (d == 0) break;
            }
            else if (match_character(ctx, sep_char) && d==1) {
                *args++ = strndup_e(ctx->buffer.buf + p, ctx->bufpos - p);
                p = ctx->bufpos;
            }
            else {
                if (!match_eol(ctx)) {
                    if (match_character(ctx, '$')) {
                        ctx->buffer.buf[ctx->bufpos - 1] = '_';
                    }
                    else {
                        match_character_any(ctx);
                    }
                }
            }
        }
		*args = NULL;
        return TRUE;
    }
    return FALSE;
}

static bool_t parse_block(context_t *ctx) {
    int l = ctx->linenum;
    int m = ctx->bufpos - ctx->linepos;
    if (match_character(ctx, '{')) {
		fputc('{', ctx->sfile);
        int d = 1;
        for (;;) {
            if (match_eof(ctx)) {
                print_error("%s:%d:%d: Premature EOF in code block\n", ctx->iname, l + 1, m + 1);
                ctx->errnum++;
                break;
            }
            if (
                match_other(ctx)
            ) continue;
            if (match_character(ctx, '{')) {
                d++;
				fputc('{', ctx->sfile);
            }
            else if (match_character(ctx, '}')) {
                d--;
				fputc('}', ctx->sfile);
                if (d == 0) break;
            }
            else {
                if (!match_eol(ctx)) {
                    if (match_character(ctx, '$')) {
                        ctx->buffer.buf[ctx->bufpos - 1] = '_';
                    }
                    else {
                        match_character_any(ctx);
                    }
                }
					fputc(ctx->buffer.buf[ctx->bufpos - 1], ctx->sfile);

            }
        }
        return TRUE;
    }
    return FALSE;
}



static const char* parse_struct(context_t *ctx) {
    int p = ctx->bufpos, q;
    int l = ctx->linenum;
    int m = ctx->bufpos - ctx->linepos;
    const char*nm = NULL, *args[20];

    if (!match_string(ctx, "struct")) goto EXCEPTION;
    match_spaces(ctx);
    q = ctx->bufpos;

    if (match_identifier(ctx)){
	    nm = strndup_e(ctx->buffer.buf + q, ctx->bufpos - q);
	    match_spaces(ctx);
	}

    if (!parse_fields(ctx, args, 20)) goto EXCEPTION;
    match_spaces(ctx);
   
	// for(int i=0; i<ctx->nespos; i++) putchar('\t');
	fprintf(ctx->hfile, "%s:%d:%d: struct %s {", ctx->iname, l + 1, m + 1, nm ? nm : "");
	for(int i=0; args[i]; i++) fprintf(ctx->hfile, "%s", args[i]);
	fprintf(ctx->hfile, "}\n");
    return nm;

EXCEPTION:;
    // destroy_node(n_r);
    ctx->bufpos = p;
    ctx->linenum = l;
    ctx->linepos = p - m;
    return NULL;
}

static const char* parse_call(context_t *ctx) {
    int p = ctx->bufpos, q;
    int l = ctx->linenum;
    int m = ctx->bufpos - ctx->linepos;
    const char*n_r = NULL, *args[20];
    if (!match_identifier(ctx)) goto EXCEPTION;
    q = ctx->bufpos;
    match_spaces(ctx);

    if (!parse_args(ctx, args, 20)) goto EXCEPTION;
    match_spaces(ctx);

    n_r = strndup_e(ctx->buffer.buf + p, q - p);
	// for(int i=0; i<ctx->nespos; i++) putchar('\t');
	fprintf(ctx->hfile, "%s:%d:%d: %s (", ctx->iname, l + 1, m + 1, n_r);
	for(int i=0; args[i]; i++) fprintf(ctx->hfile, "%s%s", args[i], args[i+1] ? ", " : "");
	fprintf(ctx->hfile, ")\n");

    return n_r;

EXCEPTION:;
    // destroy_node(n_r);
    ctx->bufpos = p;
    ctx->linenum = l;
    ctx->linepos = p - m;
    return NULL;
}



static const char* parse_function2(context_t *ctx) {
    int p = ctx->bufpos, q;
    int l = ctx->linenum;
    int m = ctx->bufpos - ctx->linepos;
     const char*n_r = NULL, *args[20];

   if (!match_identifier(ctx)) goto EXCEPTION;
    match_spaces(ctx);
    if (!parse_args(ctx, args, 20)) goto EXCEPTION;
    match_spaces(ctx);

    n_r = strndup_e(ctx->buffer.buf + p, q - p);
	fprintf(ctx->hfile, "%s:%d:%d: %s (", ctx->iname, l + 1, m + 1, n_r);
	for(int i=0; args[i]; i++) fprintf(ctx->hfile, "%s%s", args[i], args[i+1] ? ", " : "");
	fprintf(ctx->hfile, ");\n");

    if (!parse_block(ctx)) goto EXCEPTION;
    match_spaces(ctx);

    return n_r;

EXCEPTION:;
    // destroy_node(n_r);
    ctx->bufpos = p;
    ctx->linenum = l;
    ctx->linepos = p - m;
    return NULL;
}
static bool_t parse_function(context_t *ctx) {
    int p = ctx->bufpos, q, ns, ne;
    int l = ctx->linenum;
    int m = ctx->bufpos - ctx->linepos;
    const char*n_r = NULL, *args[20];
	while(!match_eof(ctx)){
		ns = ctx->bufpos;
	    if (match_identifier(ctx)){
			ne = ctx->bufpos;
		    match_spaces(ctx);
			if (parse_args(ctx, args, 20)) break;
		} else if(match_character(ctx, '*')){
		    match_spaces(ctx);
		} else {
			goto EXCEPTION;
		}
	}
    // if (!match_identifier(ctx)) goto EXCEPTION;
    // q = ctx->bufpos;
    // match_spaces(ctx);
    // if (!parse_args(ctx, args, 20)) goto EXCEPTION;
    q = ctx->bufpos;
    match_spaces(ctx);

//     n_r = strndup_e(ctx->buffer.buf + p, q - p);
// 	fprintf(ctx->hfile, "%s:%d:%d: %s (", ctx->iname, l + 1, m + 1, n_r);
// 	for(int i=0; args[i]; i++) fprintf(ctx->hfile, "%s%s", args[i], args[i+1] ? ", " : "");
// 	fprintf(ctx->hfile, ");\n");
	// fprintf(ctx->sfile, "`%.*s`", q - p, ctx->buffer.buf + p);
	long int bs = ftell(ctx->sfile);
	fprintf(ctx->sfile, "%.*s", q - p, ctx->buffer.buf + p);
	luaL_Buffer b; luaL_buffinit(L, &b);
	if( !parse_macro_block(ctx, &b) ){
   	// if (!parse_block(ctx)){
		// fseek(ctx->sfile, bs, SEEK_SET);
		goto EXCEPTION;
	} 
	
	luaL_pushresult(&b);
	fprintf(ctx->sfile, "%s", lua_tostring(L, -1));
    //match_spaces(ctx);


if( lua_getglobal(L, "onFunctionDefined") == LUA_TFUNCTION ){
	// lua_rawgeti(L, LUA_REGISTRYINDEX, (*__pcc_out));
	// createCmptObject(L, __pcc_ctx->auxil);
	lua_newtable(L);
	lua_pushlstring(L, ctx->buffer.buf + ns, ne - ns);
	lua_setfield(L, -2, "name"); 

	// lua_pushinteger(L, __pcc_in->data.leaf.capt0.range.start);
	lua_pushlstring(L, ctx->buffer.buf + p, ns - p);
	lua_setfield(L, -2, "result"); 

	luaL_pushresult(&b);
	lua_setfield(L, -2, "body"); 

	// lua_pushinteger(L, __pcc_in->data.leaf.capt0.range.end);
	// lua_newtable(L);
	// lua_rawgeti(L, LUA_REGISTRYINDEX, (*__pcc_in->data.leaf.values.buf[0]));
	// lua_setfield(L, -2, "e"); 
	int argcount;// = parse_macro_args(ctx);
	for(argcount = 0; args[argcount]; argcount++){
		lua_pushstring(L, args[argcount]);
		lua_seti(L, -2, argcount + 1); 
	}
	lua_call(L, 1, 0);    
	lua_getfield(L, -2, "result"); 
	fprintf(stderr, "%s ", lua_tostring(L, -1));
	// lua_getfield(L, -3, "name"); 
	// fprintf(stderr, "%s(", lua_tostring(L, -1));
// 	for(int i = 1; i<=argcount; i++){
// 
// 	lua_geti(L, -3, i); 
// 	fprintf(ctx->sfile, "%s  ", lua_tostring(L, -1));
// }

	// lua_getfield(L, -3, "body"); 
	// fprintf(ctx->sfile, ")%s", lua_tostring(L, -1));
	// if(!lua_isnoneornil(L, -1)) fputs(lua_tostring(L, -1), ctx->sfile);
}



	//n_r = strndup_e(ctx->buffer.buf + p, q - p);
	fprintf(ctx->hfile, "%s:%d:%d: ", ctx->iname, l + 1, m + 1);
	fprintf(ctx->hfile, "%.*s;\n", q - p, ctx->buffer.buf + p);
	// fprintf(ctx->hfile, "%s:%d:%d: %s (", ctx->iname, l + 1, m + 1, n_r);
	// for(int i=0; args[i]; i++) fprintf(ctx->hfile, "%s%s", args[i], args[i+1] ? ", " : "");
	// fprintf(ctx->hfile, ");\n");	
	// fprintf(ctx->hfile, ";\n");	
	// fprintf(ctx->sfile, "`%*s`", ctx->bufpos - p - 1, ctx->buffer.buf + p);
// fprintf(ctx->sfile, "`%s`\n//end\n", strndup_e(ctx->buffer.buf + p, ctx->bufpos - p));

    return TRUE;

EXCEPTION:;
    // destroy_node(n_r);
    ctx->bufpos = p;
    ctx->linenum = l;
    ctx->linepos = p - m;
    return FALSE;
}

int main(int argc, const char* argv[]){
			L = luaL_newstate();
			lua_newtable(L);	
			
			// ps[psi++] = (parser_t){ 
			// 	L, fd, f, 0, 0, 0, luaL_ref(L, LUA_REGISTRYINDEX), scope_new(NULL), new_idents()
			// };
			luaL_openlibs(L);





			if(luaL_dofile(L, "C:\\lib\\packcc\\test2.lua")){
				fprintf(stderr, luaL_checkstring(L, -1));
				exit(0);
			}


	context_t *ctx = create_context(
		"C:\\lib\\packcc\\test2.mc", "C:\\lib\\packcc\\test2.mc", 1);
	ctx->hfile = stdout;
    {
        const char*nm;
        //match_spaces(ctx);
        for (;;) {
            int p, l, m;
            if (match_eof(ctx) || match_footer_start(ctx)) break;
            p = ctx->bufpos;
            l = ctx->linenum;
            m = ctx->bufpos - ctx->linepos;
            if ( parse_function(ctx) ){//|| parse_struct(ctx) ) {
				//fprintf(ctx->hfile, "%s:%d:%d: %s\n", ctx->iname, l + 1, m + 1, nm);
						}else if (
							match_other(ctx)
						){ 
							//match_spaces(ctx);
							continue;
						}
            else {
							match_character_any(ctx);
fputc(ctx->buffer.buf[ctx->bufpos - 1], ctx->sfile);
// 							match_spaces(ctx);
// fprintf(ctx->sfile, "%.*s", ctx->bufpos - p, ctx->buffer.buf + p);
            }
        }
    }


	return 0;
}

struct f666_tag { int i; struct { char c; long l; } st; } x;
int f666();
//@define HASH(I) I*
int f666(){ 
	struct { int i; } si;
 	//@define LIST(I) I*

	return 666; 
}

