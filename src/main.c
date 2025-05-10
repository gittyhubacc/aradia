#include "code.h"
#include "memory.h"
#include "semantics.h"
#include "string.h"
#include "syntax.h"
#include <stdio.h>
#include <string.h>

static const int part_sz = kilobyte / 4;
static string read_source(arena *a, int argc, char **argv)
{
        char c = 0;
        int parts_read = 0;
        int bytes_read = 0;
        char *buffer = make(a, char, part_sz);

        // eventually, optionally read from other places
        while ((c = fgetc(stdin)) != EOF) {
                buffer[(parts_read * part_sz) + bytes_read++] = c;
                if (bytes_read >= part_sz) {
                        steal(a, char, part_sz);
                        bytes_read = 0;
                        parts_read++;
                }
        }

        return (string){buffer, parts_read * part_sz + bytes_read};
}

static int write_output(string output, int argc, char **argv)
{
        // eventually, optionally write to other places
        fprintf(stdout, "%.*s", output.len, output.addr);
        return 0;
}

static void debug_tokens(token_string tokens)
{
        token *tok;
        string *tok_val;
        for (int i = 0; i < tokens.len; i++) {
                tok = tokens.arr + i;
                switch (tok->type) {
                default:
                        printf("%i: %s\n", tok->line_number,
                               token_names[tok->type]);
                        break;
                case TOK_LITERAL:
                        printf("%i: %s - %li\n", tok->line_number,
                               token_names[tok->type], tok->i_value);
                        break;
                case TOK_REGISTER:
                        printf("%i: %s - %li\n", tok->line_number,
                               token_names[tok->type], tok->i_value);
                        break;
                case TOK_LABEL:
                        tok_val = &tok->s_value;
                        char mem[tok_val->len + 1];
                        memcpy(mem, tok_val->addr, tok_val->len);
                        mem[tok_val->len] = '\0';
                        printf("%i: %s - %s\n", tok->line_number,
                               token_names[tok->type], mem);
                        break;
                }
        }
}

static void debug_instruction(instr ins)
{
        switch (ins.type) {
        case I_MOV:
                printf("\tmov\n");
                break;
        case I_ADD:
                printf("\tadd\n");
                break;
        case I_JEQ:
                printf("\tjeq\n");
                break;
        case I_JMP:
                printf("\tjmp\n");
                break;
        }
}

static void debug_program(program prog)
{
        instr_seq seq;
        for (int i = 0; i < prog.len; i++) {
                seq = prog.blocks[i];
                string label = seq.label.s_value;
                printf("%.*s %i:\n", label.len, label.addr, seq.len);
                for (int j = 0; j < seq.len; j++) {
                        debug_instruction(seq.arr[j]);
                }
        }
}

int main(int argc, char **argv)
{
        char memory[8 * kilobyte];
        arena root = make_arena(memory);

        string source = read_source(&root, argc, argv);
        token_string tokens = syntactic_analysis(&root, source);
        program prog = semantic_analysis(&root, tokens);
        string code = code_generation(&root, prog);

        return write_output(code, argc, argv);
}
