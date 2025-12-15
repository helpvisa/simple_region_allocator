/*
   Copyright 2025 Daniel Brackenbury

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>

#define IMPLEMENT_REGIONS
#define DEBUG_REGIONS
#include "region.h"

/* see line 92 of https://github.com/tsoding/arena/blob/master/arena.h */
#ifdef __cplusplus
#define cast_ptr(p) (decltype(p))
#else
#define cast_ptr(...)
#endif


/* global variable to manage debugging */
int enable_debug_printing = 1;


/* structures (generic object and thread data) */
struct Object {
    float value;
    int number;
    struct Object *next;
};

struct ThreadData {
    size_t thread_multiplier;
    size_t object_multiplier;
    size_t size_multiplier;
    size_t visual_multiplier;
};


/* custom functions */
struct Object *create_object(struct Region *region) {
    struct Object *obj = cast_ptr(obj)region_alloc(region, sizeof(*obj));
    obj->next = NULL;
    return obj;
}

void debug_printf(const char *format, ...) {
    if (enable_debug_printing) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}

void *allocate_on_thread(void *arg) {
    struct ThreadData thread_data = *((struct ThreadData*)arg);

    size_t total_memory_used = 0;
    struct Region *test_region = new_region(REGION_SIZE *
                                            thread_data.size_multiplier);
    struct Region *current_region = test_region;

    debug_printf("\nCreating some objects...\n");
    struct Object *object_head = create_object(test_region);
    object_head->value = 0;
    object_head->number = 0;
    struct Object *current_object = object_head;
    for (int i = 0; i < 1024 * 1024 * thread_data.object_multiplier; i++) {
        current_object->next = create_object(test_region);
        current_object = current_object->next;
        current_object->value = (float)(i + 1) / 13.3254;
        current_object->number = i + 1;
    }
    debug_printf("\n");
    current_region = test_region;
    while (current_region) {
        total_memory_used += get_region_memory_use(current_region);
        if (enable_debug_printing) {
            print_region(current_region, 'm');
            visualize_region(current_region,
                             1024 * thread_data.visual_multiplier);
        }
        current_region = current_region->next;
    }
    debug_printf("\nTotal memory used: %.2lfkb | %.2lfmb | %.2lfgb\n",
                 (double)total_memory_used / 1024,
                 (double)total_memory_used / (1024 * 1024),
                 (double)total_memory_used / (1024 * 1024 * 1024));

    debug_printf("\nResetting regions...\n\n");
    region_reset(test_region, 1);
    current_region = test_region;
    while (current_region) {
        if (enable_debug_printing) {
            print_region(current_region, 'm');
            visualize_region(current_region,
                             1024 * thread_data.visual_multiplier);
        }
        current_region = current_region->next;
    }

    /* done testing; free up our regions and return */
    region_free(test_region);
    debug_printf("\nThread finished.\n");
    pthread_exit(NULL);
}


/* test */
int main(int argc, char *argv[]) {
    /* customize test variables */
    pthread_t *thread_ids;
    struct ThreadData thread_data = {
        .thread_multiplier = 1,
        .object_multiplier = 310,
        .size_multiplier = 512,
        .visual_multiplier = 1024 * 10
    };

    if (argc > 1) {
        enable_debug_printing = strtod(argv[1], NULL);
    }
    if (argc > 2) {
        thread_data.thread_multiplier = strtod(argv[2], NULL);
    }
    if (argc > 3) {
        thread_data.object_multiplier = strtod(argv[3], NULL);
    }
    if (argc > 4) {
        thread_data.size_multiplier = strtod(argv[4], NULL);
    }
    if (argc > 5) {
        thread_data.visual_multiplier = strtod(argv[5], NULL);
    }

    /* perform some sanity checks */
    if (thread_data.size_multiplier == 0) {
        thread_data.size_multiplier = 1;
    }
    if (thread_data.visual_multiplier == 0) {
        thread_data.visual_multiplier = 1;
    }

    debug_printf("Will make %lu objects across %lu thread(s).\n",
                 1024 * 1024 * thread_data.object_multiplier,
                 thread_data.thread_multiplier);
    debug_printf("Regions will be %luMB large.\n",
                 thread_data.size_multiplier);

    /* allocate array to manage thread IDs */
    thread_ids = (pthread_t*)malloc(thread_data.thread_multiplier *
                                    sizeof(pthread_t));
    /* initiate our test workload */
    for (int i = 0; i < thread_data.thread_multiplier; i++) {
        int return_status = pthread_create(&thread_ids[i],
                                           NULL,
                                           allocate_on_thread,
                                           (void*)&thread_data);
        if (return_status) {
            perror("Failed to create thread!");
            return 1;
        }
    }
    /* loop again, waiting for all threads to finish before exiting */
    for (int i = 0; i < thread_data.thread_multiplier; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    return 0;
}
