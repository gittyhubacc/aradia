#include "semantics.h"
#include "list.h"
#include "syntax.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AS_FLAG(T) (1 << T)
#define ASSERT_TOK(t, T)                                                       \
        {                                                                      \
                if (t.type != T) {                                             \
                        fprintf(stderr, "line %i: expected %s got %s\n",       \
                                t.line_number, token_names[T],                 \
                                token_names[t.type]);                          \
                        exit(1);                                               \
                }                                                              \
        }

static instr read_if_instr(token_string tokens, int *i)
{
        instr ins;
        ins.type = I_JEQ;
        ins.src = tokens.arr[(*i)++];
        ASSERT_TOK(ins.src, TOK_REGISTER);

        ASSERT_TOK(tokens.arr[(*i)++], TOK_JUMP);

        ins.dst = tokens.arr[(*i)++];
        if (ins.dst.type != TOK_LABEL && ins.dst.type != TOK_REGISTER) {
                fprintf(stderr,
                        "line %i: expected TOK_LABEL or TOK_REGISTER, got %s\n",
                        ins.dst.line_number, token_names[ins.dst.type]);
                exit(1);
        }

        ASSERT_TOK(tokens.arr[(*i)++], TOK_SEMICOLON);

        return ins;
}
static instr read_jmp_instr(token_string tokens, int *i)
{
        instr ins;
        ins.type = I_JMP;
        ins.dst = tokens.arr[(*i)++];
        ASSERT_TOK(ins.dst, TOK_LABEL);

        return ins;
}
static instr read_addmov_instr(token_string tokens, int *i)
{
        instr ins;
        ins.dst = tokens.arr[(*i) - 1];
        ASSERT_TOK(ins.dst, TOK_REGISTER); // redundant

        ASSERT_TOK(tokens.arr[(*i)++], TOK_EQUAL);

        // check the token next after the one we're pointing to
        token left = tokens.arr[(*i)++];
        switch (tokens.arr[(*i)++].type) {
        case TOK_SEMICOLON:
                ins.type = I_MOV;
                if (left.type != TOK_LITERAL && left.type != TOK_LABEL &&
                    left.type != TOK_REGISTER) {
                        fprintf(stderr,
                                "line %i: expected TOK_LABEL or TOK_LITERAL or "
                                "TOK_REGISTER, got %s",
                                left.line_number, token_names[left.type]);
                        exit(1);
                }
                ins.src = left;
                return ins;
        case TOK_PLUS:
                ins.type = I_ADD;
                ins.src = tokens.arr[(*i)++];
                if (ins.src.type != TOK_LITERAL && ins.src.type != TOK_LABEL &&
                    ins.src.type != TOK_REGISTER) {
                        fprintf(stderr,
                                "line %i: expected TOK_LABEL or TOK_LITERAL or "
                                "TOK_REGISTER, got %s",
                                ins.src.line_number, token_names[ins.src.type]);
                        exit(1);
                }
                ASSERT_TOK(tokens.arr[(*i)++], TOK_SEMICOLON);
                return ins;
        default:
                left = tokens.arr[(*i) - 1];
                fprintf(stderr,
                        "line %i: expected TOK_SEMICOLON or TOK_PLUS, got %s\n",
                        left.line_number, token_names[left.type]);
                exit(1);
        }
}
static instr read_instr(token_string tokens, int *i)
{
        token t = tokens.arr[(*i)++];
        switch (t.type) {
        case TOK_IF:
                return read_if_instr(tokens, i);
        case TOK_JUMP:
                return read_jmp_instr(tokens, i);
        case TOK_REGISTER:
                return read_addmov_instr(tokens, i);
        default:
                fprintf(stderr,
                        "line %i: expected (TOK_IF, TOK_JUMP, TOK_REGISTER) "
                        "got %s\n",
                        t.line_number, token_names[t.type]);
                exit(1);
        }
}

static instr_seq *read_instr_seq(arena *a, token_string tokens, int *i)
{
        token label_tok = tokens.arr[(*i)++];
        ASSERT_TOK(label_tok, TOK_LABEL);

        ASSERT_TOK(tokens.arr[(*i)++], TOK_COLON);

        instr_seq *seq = make(a, instr_seq, 1);
        seq->arr = make(a, instr, 1);
        seq->label = label_tok;
        seq->len = 0;

        // read bulk of block
        while (tokens.arr[*i].type != TOK_JUMP) {
                steal(a, instr_seq, 1);
                seq->arr[seq->len++] = read_instr(tokens, i);
        }

        // read last jump instruction
        steal(a, instr_seq, 1);
        seq->arr[seq->len++] = read_instr(tokens, i);

        return seq;
}

program semantic_analysis(arena *a, token_string tokens)
{
        char tmp_mem[1 * kilobyte];
        arena tmp = make_arena(tmp_mem);

        int i = 0;
        int seq_cnt = 0;
        instr_seq *i_seq;
        list_node *node = NULL;
        list_node *instr_seq_list = NULL;
        while (i < tokens.len) {
                i_seq = read_instr_seq(a, tokens, &i);
                node = make(&tmp, list_node, 1);
                node->next = NULL;
                node->data = i_seq;
                list_append(&instr_seq_list, node);
                seq_cnt++;
        }

        program prog;
        prog.len = seq_cnt;
        prog.blocks = make(a, instr_seq, prog.len);

        seq_cnt = 0;
        node = instr_seq_list;
        while (node) {
                prog.blocks[seq_cnt++] = *((instr_seq *)node->data);
                node = node->next;
        }

        return prog;
}
