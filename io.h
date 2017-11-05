#ifndef _IO_H_
#define _IO_H_

#include <string.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
	FILE* raw;
	int lines;
} _FILE;

_FILE * _fopen(const char *filename, const char *mode);
_FILE * _fopen(const char *filename, const char *mode){
	FILE* raw = fopen(const char *filename, const char *mode);
	if(!raw) return NULL;
	_FILE * f = malloc(sizeof(_FILE));
	f->raw = raw;
	f->lines = 0;
	return f;
}

void _fclose(_FILE* f){
	fclose(f->raw);
	free(f);
}

void _fputs(const char *str, _FILE* f){
	for(const char *s = str; *s; s++) 
		if(*s=='\n') f->lines++;
	fputs(str, f->raw);
}

void _fputc(char c, _FILE* f){
	if(c=='\n') f->lines++;
	fputc(c, f->raw);
}

#define _fprintf(F, ...) fprintf((F)->raw, __VA_ARGS__)

#endif // _IO_H_