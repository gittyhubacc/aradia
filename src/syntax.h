#ifndef flcl_syntax
#define flcl_syntax

#include <stdint.h>
#include "memory.h"
#include "string.h"

enum token_type {
	TOK_BROKEN,

	TOK_LABEL,
	TOK_LITERAL,
	TOK_REGISTER,

	TOK_IF,
	TOK_JUMP,

	TOK_PLUS,
	TOK_EQUAL,
	TOK_COLON,
	TOK_SEMICOLON,
};

extern const char *token_names[];

typedef struct token {
	enum token_type type;
	union {
		string s_value;
		double d_value;
		int64_t i_value;
	};
} token;

typedef struct token_string {
	token *arr;
	int len;
} token_string;

token_string syntactic_analysis(arena *a, string source);

#endif
