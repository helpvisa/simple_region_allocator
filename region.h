#ifndef REGION_H_
#define REGION_H_
#include <stdlib.h>

/* set a default allocation size for arenas that can be overridden */
#ifndef REGION_SIZE
#define REGION_SIZE (1024 * 1024)
#endif

/* structures */
struct Region {
    size_t capacity;
    size_t size;
    void *data;

    struct Region *next;
};
/* forward-declare functions */
struct Region *new_region(size_t size);
void *region_alloc(struct Region *region, size_t size);

#ifdef DEBUG_REGIONS
#include <stdio.h>
void print_region(struct Region *region, char format);
void visualize_region(struct Region *region, int step);
#endif

/* use #define IMPLEMENT_REGIONS in your source file */
#ifdef IMPLEMENT_REGIONS
/* malloc a new memory region and return it */
struct Region *new_region(size_t size) {
    void *memory = malloc(size);
    if (!memory) {
        return NULL;
    }

    /* store the Region struct at the beginning of its own allocated memory */
    struct Region *region = (struct Region *)memory;
    region->capacity = size - sizeof(*region);
    /* offset the data pointer to skip the self-allocated region */
    region->data = (char *)memory + sizeof(*region);
    region->size = 0;

    return region;
}

/* allocate within a given region */
void *region_alloc(struct Region *region, size_t size) {
    void *new_allocation = NULL;

    if (size > 0 && region->capacity - region->size >= size) {
        new_allocation = region->data + region->size;
        region->size += size;
    }

    return new_allocation;
}
#endif

#ifdef DEBUG_REGIONS
/* print memory used by region */
void print_region(struct Region *region, char format) {
    int size = region->size;
    int capacity = region->capacity;
    int free = region->capacity - region->size;
    char* postfix;

    switch (format) {
    case 'k':
        size /= 1024;
        capacity /= 1024;
        free /= 1024;
        postfix = "kb\0";
        break;
    case 'm':
        size /= (1024 * 1024);
        capacity /= (1024 * 1024);
        free /= (1024 * 1024);
        postfix = "mb\0";
        break;
    default:
        postfix = "b\0";
        break;
    }

    printf("%d%s used of %d%s total; %d%s free\n",
           size, postfix,
           capacity, postfix,
           free, postfix);
}

/* visualize a memory region with ASCII art */
void visualize_region(struct Region *region, int step) {
    for (int i = 0; i < region->size; i += step) {
        printf("#");
    }
    for (int i = 0; i < region->capacity - region->size; i += step) {
        printf(".");
    }
    printf("\n");
}
#endif

#endif
