#include "oslabs.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

struct MEMORY_BLOCK best_fit_allocate(int request_size,
                                      struct MEMORY_BLOCK memory_map[MAPMAX],
                                      int *map_cnt,
                                      int process_id) {
    // NULLBLOCK
    struct MEMORY_BLOCK nullblock = {0, 0, 0, 0};

    int best_index = -1;
    int best_size = 0;

    // Find best-fit free block
    for (int i = 0; i < *map_cnt; i++) {
        // Only consider free blocks
        if (memory_map[i].process_id != 0)
            continue;

        // Must be large enough
        if (memory_map[i].segment_size < request_size)
            continue;

        // First suitable block
        if (best_index == -1) {
            best_index = i;
            best_size = memory_map[i].segment_size;
        } else {
            // Choose the block with the smallest segment_size >= request_size
            if (memory_map[i].segment_size < best_size) {
                best_index = i;
                best_size = memory_map[i].segment_size;
            }
        }

        // Early exit if we found a perfect fit
        if (best_size == request_size) {
            break;
        }
    }

    // No suitable block found
    if (best_index == -1) {
        return nullblock;
    }

    struct MEMORY_BLOCK chosen = memory_map[best_index];

    // Case 1: exact fit, just mark as allocated
    if (chosen.segment_size == request_size) {
        memory_map[best_index].process_id = process_id;
        return memory_map[best_index];
    }

    // Case 2: split the block
    struct MEMORY_BLOCK allocated;
    allocated.start_address = chosen.start_address;
    allocated.end_address   = allocated.start_address + request_size - 1;
    allocated.segment_size  = request_size;
    allocated.process_id    = process_id;

    // Make room for the new free block after the allocated one
    for (int i = *map_cnt; i > best_index; i--) {
        memory_map[i] = memory_map[i - 1];
    }
    (*map_cnt)++;

    // Put allocated block in its place
    memory_map[best_index] = allocated;

    // Create the remaining free block
    memory_map[best_index + 1].start_address = allocated.end_address + 1;
    memory_map[best_index + 1].end_address   = chosen.end_address;
    memory_map[best_index + 1].segment_size  = chosen.segment_size - request_size;
    memory_map[best_index + 1].process_id    = 0;

    return allocated;
}
