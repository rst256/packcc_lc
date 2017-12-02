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


bool_t is_line_first(context_t *ctx, int p){
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

bool_t match_directive(context_t *ctx){
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



static bool_t parse_args(context_t *ctx, const char** args, size_t args_size){
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