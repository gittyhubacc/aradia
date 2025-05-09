#include <stdio.h>
#include <string.h>
#include "string.h"
#include "list.h"
#include "memory.h"
#include "syntax.h"

static const int part_sz = kilobyte / 4;
static string read_source(arena *a, int argc, char **argv)
{
	char c = 0;
	int parts_read = 0;
	int bytes_read = 0;
	char *buffer = make(a, char, part_sz);

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

int main(int argc, char **argv)
{
	token *tok;
	string *tok_val;
	char memory[8 * kilobyte];
	arena root = make_arena(memory);

	string source = read_source(&root, argc, argv);
	token_string tokens = syntactic_analysis(&root, source);

	for (int i = 0; i < tokens.len; i++) {
		tok = tokens.arr + i;
		switch (tok->type) {
		default:
			printf("%s\n", token_names[tok->type]);
			break;
		case TOK_LITERAL:
			printf("%s - %li\n", token_names[tok->type], tok->i_value);
			break;
		case TOK_REGISTER:
			printf("%s - %li\n", token_names[tok->type], tok->i_value);
			break;
		case TOK_LABEL:
			tok_val = &tok->s_value;
			char mem[tok_val->len + 1];
			memcpy(mem, tok_val->addr, tok_val->len);
			mem[tok_val->len] = '\0';
			printf("%s - %s\n", token_names[tok->type], mem);
			break;
		}
	}

	return 0;
}
