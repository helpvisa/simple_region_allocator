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
    /* 256MB regions */
    struct Region *test_region = new_region(REGION_SIZE * 256);
    struct Region *current_region = test_region;

    printf("\nCreating some objects...\n");
    struct Object *object_head = create_object(test_region);
    object_head->value = 0;
    object_head->number = 0;
    struct Object *current_object = object_head;
    for (int i = 0; i < 1024 * 1024 * 310; i++) {
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
        print_region(current_region, 'm');
        visualize_region(current_region, 1024 * 1024 * 8);
        current_region = current_region->next;
    }

    printf("\nResetting regions...\n");
    region_reset(test_region);
    current_region = test_region;
    while (current_region) {
        print_region(current_region, 'm');
        visualize_region(current_region, 1024 * 1024 * 8);
        current_region = current_region->next;
    }

    /* done testing; free up our regions and return */
    region_free(test_region);
    return 0;
}
