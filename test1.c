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

#include "packcc.h"
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
                match_directive(ctx) ||
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
                match_directive(ctx) ||
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


static bool_t parse_block(context_t *ctx) {
    int l = ctx->linenum;
    int m = ctx->bufpos - ctx->linepos;
    if (match_character(ctx, '{')) {
        int d = 1;
        for (;;) {
            if (match_eof(ctx)) {
                print_error("%s:%d:%d: Premature EOF in code block\n", ctx->iname, l + 1, m + 1);
                ctx->errnum++;
                break;
            }
            if (
                match_directive(ctx) ||
                match_comment_c(ctx) ||
                match_comment_cxx(ctx) ||
                match_quotation_single(ctx) ||
                match_quotation_double(ctx)
            ) continue;
            if (match_character(ctx, '{')) {
                d++;
            }
            else if (match_character(ctx, '}')) {
                d--;
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
            }
        }
        return TRUE;
    }
    return FALSE;
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
static const char* parse_function(context_t *ctx) {
    int p = ctx->bufpos, q;
    int l = ctx->linenum;
    int m = ctx->bufpos - ctx->linepos;
    const char*n_r = NULL, *args[20];
	while(!match_eof(ctx)){
	    if (match_identifier(ctx)){
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

    if (!parse_block(ctx)) goto EXCEPTION;
    // match_spaces(ctx);

	n_r = strndup_e(ctx->buffer.buf + p, q - p);
	fprintf(ctx->hfile, "%s:%d:%d: ", ctx->iname, l + 1, m + 1);
	fprintf(ctx->hfile, "%s;\n", n_r);
	// fprintf(ctx->hfile, "%s:%d:%d: %s (", ctx->iname, l + 1, m + 1, n_r);
	// for(int i=0; args[i]; i++) fprintf(ctx->hfile, "%s%s", args[i], args[i+1] ? ", " : "");
	// fprintf(ctx->hfile, ");\n");	
	// fprintf(ctx->hfile, ";\n");	

    return n_r;

EXCEPTION:;
    // destroy_node(n_r);
    ctx->bufpos = p;
    ctx->linenum = l;
    ctx->linepos = p - m;
    return NULL;
}

int main(int argc, const char* argv[]){
	context_t *ctx = create_context(
		"C:\\lib\\packcc\\test1.c", "C:\\lib\\packcc\\test1.out.c", 1);
	ctx->hfile = stdout;
    {
        const char*nm;
        match_spaces(ctx);
        for (;;) {
            int p, l, m;
            if (match_eof(ctx) || match_footer_start(ctx)) break;
            p = ctx->bufpos;
            l = ctx->linenum;
            m = ctx->bufpos - ctx->linepos;
            if ( parse_function(ctx) ){//|| parse_struct(ctx) ) {
				//fprintf(ctx->hfile, "%s:%d:%d: %s\n", ctx->iname, l + 1, m + 1, nm);
						}else if (
							match_directive(ctx) ||
							match_comment_c(ctx) ||
							match_comment_cxx(ctx) ||
							match_quotation_single(ctx) ||
							match_quotation_double(ctx)
						){ 
							match_spaces(ctx);
							continue;
						}
            else {
							match_character_any(ctx);
							//match_spaces(ctx);
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