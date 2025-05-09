#ifndef flcl_memory
#define flcl_memory

#define kilobyte (1024)

typedef struct arena {
	char *base;
	char *end;
	int sz;
} arena;

char *arena_alloc(arena *a, int size, int count, int align);
void arena_steal(arena *a, int size, int count);
#define make_arena(m) ((arena){m, m + sizeof(m), sizeof(m)})
#define make(a, T, c) ((T *)arena_alloc(a, sizeof(T), c, _Alignof(T)))
#define steal(a, T, c) (arena_steal(a, sizeof(T), c))

#endif
