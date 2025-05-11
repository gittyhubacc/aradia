#ifndef flcl_syntax
#define flcl_syntax

#include "memory.h"
#include "string.h"
#include <stdint.h>

enum token_type {
        TOK_BROKEN,

        TOK_LABEL,
        TOK_LITERAL,
        TOK_REGISTER,

        TOK_IF,
        TOK_JUMP,
        TOK_EMIT,

        TOK_PLUS,
        TOK_EQUAL,
        TOK_COLON,
        TOK_SEMICOLON,
};

extern const char *token_names[];

typedef struct token {
        enum token_type type;
        int line_number;
        string s_value;
        union {
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
