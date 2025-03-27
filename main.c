#include <stdio.h>

#define IMPLEMENT_REGIONS
#define DEBUG_REGIONS
#include "region.h"

struct Object {
    float value;
    int number;
    struct Object *next;
};

struct Object *create_object(struct Region *region) {
    struct Object *new_object = region_alloc(region, sizeof(*new_object));
    new_object->next = NULL;
    return new_object;
}

int main(int argc, char *argv[]) {
    /* customize region size */
    size_t object_multiplier = 310;
    size_t size_multiplier = 512;
    size_t visual_multiplier = 1024 * 10;
    if (argc > 1) {
        object_multiplier = strtod(argv[1], NULL);
    }
    if (argc > 2) {
        size_multiplier = strtod(argv[2], NULL);
    }
    if (argc > 3) {
        visual_multiplier = strtod(argv[3], NULL);
    }
    if (size_multiplier == 0) {
        size_multiplier = 1;
    }
    if (visual_multiplier == 0) {
        visual_multiplier = 1;
    }

    printf("Will make %lu objects.\n",
            1024 * 1024 * object_multiplier);
    printf("Regions will be %luMB large.\n", size_multiplier);

    /* 512MB regions by default */
    size_t total_memory_used = 0;
    struct Region *test_region = new_region(REGION_SIZE * size_multiplier);
    struct Region *current_region = test_region;

    printf("\nCreating some objects...\n");
    struct Object *object_head = create_object(test_region);
    object_head->value = 0;
    object_head->number = 0;
    struct Object *current_object = object_head;
    for (int i = 0; i < 1024 * 1024 * object_multiplier; i++) {
        current_object->next = create_object(test_region);
        current_object = current_object->next;
        current_object->value = (float)(i + 1) / 13.3254;
        current_object->number = i + 1;
    }
    /* current_object = object_head; */
    /* while (current_object) { */
    /*     printf("Object %d has value %f\n", */
    /*            current_object->number, current_object->value); */
    /*     current_object = current_object->next; */
    /* } */
    printf("\n");
    current_region = test_region;
    while (current_region) {
        total_memory_used += get_region_memory_use(current_region);
        print_region(current_region, 'm');
        visualize_region(current_region, 1024 * visual_multiplier);
        current_region = current_region->next;
    }
    printf("\nTotal memory used: %.2lfkb | %.2lfmb | %.2lfgb\n",
           (double)total_memory_used / 1024,
           (double)total_memory_used / (1024 * 1024),
           (double)total_memory_used / (1024 * 1024 * 1024));

    printf("\nResetting regions...\n\n");
    region_reset(test_region);
    current_region = test_region;
    while (current_region) {
        print_region(current_region, 'm');
        visualize_region(current_region, 1024 * visual_multiplier);
        current_region = current_region->next;
    }

    /* done testing; free up our regions and return */
    region_free(test_region);
    return 0;
}
