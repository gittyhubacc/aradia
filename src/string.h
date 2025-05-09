#ifndef flcl_string
#define flcl_string

typedef struct string {
	char *addr;
	int len;
	// implicit sz of 1
} string;

#define S(s) ((string){s, sizeof(s) - 1})
#define S_NULL ((string){0, 0})

#define S_ADD0 (/*not ready*/)
/*
	tok_val = &tok->s_value;
	char mem[tok_val->len + 1];
	memcpy(mem, tok_val->addr, tok_val->len);
	mem[tok_val->len] = '\0';
*/

#endif
