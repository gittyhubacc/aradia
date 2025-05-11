#ifndef flcl_semantics
#define flcl_semantics

#include "memory.h"
#include "syntax.h"

enum instr_type { I_MOV, I_ADD, I_JEQ, I_JMP, I_PUT };

typedef struct instr {
        enum instr_type type;
        token dst;
        token src;
} instr;

typedef struct instr_seq {
        token label;
        instr *arr;
        int len;
} instr_seq;

typedef struct program {
        instr_seq *blocks;
        int len;
} program;

program semantic_analysis(arena *a, token_string tokens);

#endif
