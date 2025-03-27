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
void region_reset(struct Region *region);
void region_free(struct Region *region);

#ifdef DEBUG_REGIONS
#include <stdio.h>
size_t get_region_memory_use(struct Region *region);
void print_region(struct Region *region, char format);
void visualize_region(struct Region *region, int step);
#endif

/* use #define IMPLEMENT_REGIONS in your source file */
#ifdef IMPLEMENT_REGIONS
/* malloc a new memory region and return it */
struct Region *new_region(size_t size) {
    void *memory = malloc(size);
    if (!memory) {
        abort();
        /* return NULL; */
    }

    /* store the Region struct at the beginning of its own allocated memory */
    struct Region *region = (struct Region *)memory;
    region->capacity = size;
    /* offset the data pointer to skip the self-allocated region */
    region->data = (char *)memory;
    region->size = sizeof(*region);
    region->next = NULL;

    return region;
}

/* allocate within a given region */
void *region_alloc(struct Region *region, size_t size) {
    void *new_allocation = NULL;

    if (size < 1 || size > region->capacity - sizeof(*region)) {
#ifdef DEBUG_REGIONS
        printf("ERR: attempted to insert an object with no size, "
               "or an object larger than maximum arena capacity.\n");
#endif
        return new_allocation;
    }

    if (region->capacity - region->size >= size) {
        new_allocation = region->data + region->size;
        region->size += size;
    } else {
        if (!region->next) {
            region->next = new_region(region->capacity);
        }
        new_allocation = region_alloc(region->next, size);
    }
    return new_allocation;
}

/* clear a given region by resetting its size to default */
void region_reset(struct Region *region) {
    region->size = sizeof(*region);
    /* if there's another region linked to this,
       might as well go ahead and free it, since
       it's only accessed via this one */
    if (region->next) {
        region_free(region->next);
        region->next = NULL;
    }
}

/* free a given region, and all of its connected regions */
void region_free(struct Region *region) {
    if (region->next) {
        region_free(region->next);
        region->next = NULL;
    }
    /* region is stored within itself, so freeing region itself also
       automatically frees the data pointer */
    free(region);
}
#endif

#ifdef DEBUG_REGIONS
/* return the amount of memory consumed by region (in bytes) */
size_t get_region_memory_use(struct Region *region) {
    return region->size;
}

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

    printf("%d%s used | %d%s free | %d%s total\n",
           size, postfix,
           free, postfix,
           capacity, postfix);
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
