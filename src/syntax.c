#include "syntax.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *token_names[] = {"TOK_BROKEN",

                             "TOK_LABEL",    "TOK_LITERAL", "TOK_REGISTER",

                             "TOK_IF",       "TOK_JUMP",

                             "TOK_PLUS",     "TOK_EQUAL",   "TOK_COLON",
                             "TOK_SEMICOLON"};

typedef struct tokrecm {
        int state;
        int accept;
        int priority;
} tokrecm;

static void advance_plus(tokrecm *m, char c)
{
        switch (m->state) {
        case 0:
                m->accept = c == '+';
                m->state++;
                break;
        default:
                m->accept = 0;
                break;
        }
}

static void advance_equal(tokrecm *m, char c)
{
        switch (m->state) {
        case 0:
                m->accept = c == '=';
                m->state++;
                break;
        default:
                m->accept = 0;
                break;
        }
}

static void advance_colon(tokrecm *m, char c)
{
        switch (m->state) {
        case 0:
                m->accept = c == ':';
                m->state++;
                break;
        default:
                m->accept = 0;
                break;
        }
}

static void advance_semicolon(tokrecm *m, char c)
{
        switch (m->state) {
        case 0:
                m->accept = c == ';';
                m->state++;
                break;
        default:
                m->accept = 0;
                break;
        }
}

static void advance_if(tokrecm *m, char c)
{
        enum states {
                read_i,
                read_f,
                accepted,
                dead,
        };
        switch (m->state) {
        case read_i:
                m->state = c == 'i' ? read_f : dead;
                break;
        case read_f:
                m->accept = c == 'f';
                m->state = m->accept ? accepted : dead;
                break;
        case accepted:
                m->accept = 0;
                m->state = dead;
                break;
        }
}

static void advance_jump(tokrecm *m, char c)
{
        enum states { read_j, read_u, read_m, read_p, accepted, dead };
        switch (m->state) {
        case read_j:
                m->accept = c == 'j';
                m->state = c ? read_u : dead;
                break;
        case read_u:
                m->accept = c == 'u';
                m->state = m->accept ? read_m : dead;
                break;
        case read_m:
                m->accept = c == 'm';
                m->state = m->accept ? read_p : dead;
                break;
        case read_p:
                m->accept = c == 'p';
                m->state = m->accept ? accepted : dead;
                break;
        case accepted:
                m->accept = 0;
                m->state = dead;
                break;
        }
}

static int is_letter(char c)
{
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static int is_number(char c) { return c >= '0' && c <= '9'; }

static int is_whitespace(char c) { return c == ' ' || c == '\t' || c == '\n'; }

static void advance_literal(tokrecm *m, char c)
{
        enum states { max_left, left, right, dead };
        switch (m->state) {
        case max_left:
                if (is_number(c) || c == '-') {
                        m->accept = 1;
                        m->state = left;
                } else if (c == '.') {
                        m->accept = 1;
                        m->state = right;
                } else {
                        m->state = dead;
                }
                break;
        case left:
                if (is_number(c)) {
                        m->accept = 1;
                } else if (c == '.') {
                        m->state = right;
                } else {
                        m->accept = 0;
                        m->state = dead;
                }
                break;
        case right:
                if (is_number(c)) {
                        m->accept = 1;
                } else {
                        m->accept = 0;
                        m->state = dead;
                }
                break;
        }
}

static void advance_label(tokrecm *m, char c)
{
        m->accept = (!m->state) && !(m->state = !(is_letter(c) || c == '_'));
}

static void advance_register(tokrecm *m, char c)
{
        enum states { read_r, read_n, accept, dead };
        switch (m->state) {
        case read_r:
                m->state = c == 'r' ? read_n : dead;
                break;
        case read_n:
                m->accept = is_number(c);
                m->state = m->accept ? accept : dead;
                break;
        case accept:
                m->accept = 0;
                m->state = dead;
                break;
        }
}

static tokrecm label_m;
static tokrecm literal_m;
static tokrecm register_m;

static tokrecm if_m;
static tokrecm jump_m;

static tokrecm plus_m;
static tokrecm equal_m;
static tokrecm colon_m;
static tokrecm semicolon_m;

static const int machine_count = 9;

static tokrecm *machines[] = {&label_m, &literal_m, &register_m,

                              &if_m,    &jump_m,

                              &plus_m,  &equal_m,   &colon_m,    &semicolon_m};

static void reset_machine(tokrecm *m, int p)
{
        m->state = 0;
        m->accept = 0;
        m->priority = p;
}

static void reset_machines()
{
        reset_machine(&label_m, -1);
        reset_machine(&literal_m, 0);
        reset_machine(&register_m, 0);

        reset_machine(&if_m, 0);
        reset_machine(&jump_m, 0);

        reset_machine(&plus_m, 0);
        reset_machine(&equal_m, 0);
        reset_machine(&colon_m, 0);
        reset_machine(&semicolon_m, 0);
}

static void step_machines(char c)
{
        advance_label(&label_m, c);
        advance_literal(&literal_m, c);
        advance_register(&register_m, c);

        advance_if(&if_m, c);
        advance_jump(&jump_m, c);

        advance_plus(&plus_m, c);
        advance_equal(&equal_m, c);
        advance_colon(&colon_m, c);
        advance_semicolon(&semicolon_m, c);
}

static enum token_type recognize_token(string src, int *i)
{
        tokrecm *mach;
        int accepting = 999;
        int candidate_priority = -999;
        enum token_type candidate = TOK_BROKEN;

        reset_machines();

        while (accepting > 0 && *i < src.len) {
                step_machines(src.addr[(*i)++]);
                accepting = 0;
                for (int m = 0; m < machine_count; m++) {
                        mach = machines[m];
                        accepting += mach->accept;
                        if (mach->accept &&
                            mach->priority > candidate_priority) {
                                candidate = m + 1; // coincidence, for now!
                                candidate_priority = mach->priority;
                        }
                }
        }

        (*i)--; // rewind the symbol that caused us to recognize a token

        return candidate;
}

static token *read_token(arena *a, string src, int *i)
{
        int len;
        int start = *i;
        token *tok = make(a, token, 1);
        tok->type = recognize_token(src, i);

        switch (tok->type) {
        default:
                break;
        case TOK_BROKEN:
                tok->s_value.addr = src.addr + start;
                tok->s_value.len = 1; // this might not be safe
                break;
        case TOK_LABEL:
                tok->s_value.addr = src.addr + start;
                tok->s_value.len = *i - start;
                break;
        case TOK_REGISTER:
                tok->s_value.addr = src.addr + start;
                tok->s_value.len = *i - start;
                tok->i_value = src.addr[start + 1] - '0';
                break;
        case TOK_LITERAL:
                tok->s_value.addr = src.addr + start;
                tok->s_value.len = *i - start;
                len = *i - start;
                char mem[len + 1];
                memcpy(mem, src.addr + start, len);
                mem[len] = '\0';

                tok->i_value = atol(mem);
                break;
        }

        return tok;
}

token_string syntactic_analysis(arena *a, string source)
{
        char tmp_mem[3 * kilobyte];
        arena tmp = make_arena(tmp_mem);

        int tok_cnt = 0;
        token *tok = NULL;
        list_node *node = NULL;
        list_node *tok_list = NULL;

        int i = 0;
        int line_num = 1;
        while (i < source.len) {
                if (is_whitespace(source.addr[i])) {
                        line_num += source.addr[i++] == '\n';
                        continue;
                }

                tok = read_token(&tmp, source, &i);
                if (tok->type == TOK_BROKEN) {
                        if (tok->s_value.addr[0] == '#') {
                                while (source.addr[i] != '\n')
                                        i++;
                                continue;
                        }
                        char mem[tok->s_value.len + 1];
                        memcpy(mem, tok->s_value.addr, tok->s_value.len);
                        mem[tok->s_value.len] = '\0';
                        fprintf(stderr, "line %i: illegal token '%s'\n",
                                line_num, mem);
                        exit(1);
                }

                tok->line_number = line_num;

                node = make(&tmp, list_node, 1);
                node->next = 0;
                node->data = tok;
                list_append(&tok_list, node);

                tok_cnt++;
        }

        token_string tokens;
        tokens.len = tok_cnt;
        tokens.arr = make(a, token, tokens.len);

        tok_cnt = 0;
        node = tok_list;
        while (node) {
                // copy each token
                // transfer strings here?
                tokens.arr[tok_cnt++] = *((token *)node->data);
                node = node->next;
        }

        return tokens;
}
