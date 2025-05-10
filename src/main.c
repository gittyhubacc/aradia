#include "code.h"
#include "memory.h"
#include "semantics.h"
#include "string.h"
#include "syntax.h"
#include <stdio.h>

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
