#include <stdio.h>
#include <stdlib.h>

// define a 1MB arena
#define ARENA_SIZE (1024 * 1024)
char arena_mem[ARENA_SIZE];

// structure of our static Arena
struct StaticArena {
    size_t capacity;
    void *data;
    size_t size;
    size_t free;
};

struct StaticArena *new_arena(void *memory, size_t size) {
    struct StaticArena *arena = (struct StaticArena *)memory;
    arena->capacity = size - sizeof(*arena);
    // add the arena structure itself to the block of allocated memory
    arena->data = (char *)memory + sizeof(*arena);
    arena->size = 0;
    arena->free = arena->capacity;
    
    return arena;
}

void *arena_alloc(struct StaticArena *arena, size_t size) {
    void *new_allocation = NULL;

    if (size > 0 && arena->free >= size) {
        // return a pointer for this object
        new_allocation = arena->data + arena->size;
        // push the arena pointer forward
        arena->size += size;
        arena->free -= size;
    }

    return new_allocation;
}

void set_string(char *to_set, char *input, int size) {
    int i = 0;
    while (i < size && input[i] != '\0') {
        to_set[i] = input[i];
        i++;
    }
}

int main(int argc, char *argv[]) {
    struct StaticArena *test_arena = new_arena(arena_mem, ARENA_SIZE);

    char *string1 = arena_alloc(test_arena, 24);
    char *string2 = arena_alloc(test_arena, 24);

    set_string(string2, "Isn't it grand?", 24);
    set_string(string1, "This is a lot of fun!!!", 24);

    printf("%s\n%s\n", string1, string2);

    return 0;
}
