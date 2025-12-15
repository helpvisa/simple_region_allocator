# Simple Region Allocator
Basic region / stack allocator for your C and C++ projects.

## Using the Library
Include `region.h` in every file that it's needed. Make sure to include
`#define IMPLEMENT_REGIONS` *before* including region.h in your entrypoint. If
you need access to debug functions, you can also include `#define DEBUG_REGIONS`
in your entrypoint.

Regions are 1MB in size by default; you can redefine `REGION_SIZE` or use a
multiplier as desired to change the allocation size. See `main.c` included in
this repository for an example implementation.

## Included Demo
Type `make test` to see the included example in action, which allocates 4GiB of
structures and prints some debug information about memory usage.

You can also modify the parameters by providing the `arenas` binary with some
additional arguments, as follows:
```
./arenas [ENABLE_DEBUG_PRINTING] \
         [NUMBER_OF_THREADS]     \
         [OBJECT_MULTIPLIER]     \
         [VISUAL_SCALE]
```
