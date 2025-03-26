#include <stdio.h>

#define IMPLEMENT_REGIONS
#define DEBUG_REGIONS
#include "region.h"

void set_string(char *to_set, char *input, int size) {
    int i = 0;
    while (i < size - 1 && input[i] != '\0') {
        to_set[i] = input[i];
        i++;
    }
    to_set[i] = '\0';
}

int main(int argc, char *argv[]) {
    struct Region *test_region = new_region(REGION_SIZE);

    char *string1 = region_alloc(test_region, 1024 * 64);

    print_region(test_region, 'k');
    visualize_region(test_region, 1024 * 16);

    return 0;
}
