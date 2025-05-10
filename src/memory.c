#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"

char *arena_alloc(arena *a, int size, int count, int align)
{
	int padding = (long)a->base % align;
	if (a->base + padding + (size * count) >= a->end) {
		// we are super fucked here
		// TODO: report more information to help debug
		fprintf(stderr, "out of memory: %s\n", a->name);
		exit(1);
	}

	char *memory = a->base + padding;
	a->base += padding + (size * count);
	memset(memory, 0, size * count);
	return memory;
}

void arena_steal(arena *a, int size, int count)
{
	a->base += (size * count);
}
