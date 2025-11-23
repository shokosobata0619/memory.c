#include "oslabs.h"
#include <stdbool.h>

struct MEMORY_BLOCK best_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id){
    struct MEMORY_BLOCK temp_memory_block, allocated_memory;
    allocated_memory.start_address = 0;
    allocated_memory.end_address = 0;
    allocated_memory.segment_size = 0;
    allocated_memory.process_id = 0;  // NULLBLOCK

    bool match = false;
    int memory_map_index = 0;
    int best_fit_segment = 0;

    // Find free block whose size is closest (>= request_size)
    for(int i = 0; i < *map_cnt; i++){
        if (memory_map[i].process_id != 0) {
            continue; // only free blocks
        }

        if (memory_map[i].segment_size < request_size) {
            continue; // too small
        }

        if (match == false) {
            // first suitable block
            match = true;
            memory_map_index = i;
            best_fit_segment = memory_map[i].segment_size;
        } else {
            // pick smaller block (best fit)
            if (memory_map[i].segment_size < best_fit_segment) {
                memory_map_index = i;
                best_fit_segment = memory_map[i].segment_size;
            }
        }
    }

    if (match == true){
        temp_memory_block = memory_map[memory_map_index];

        // Exact fit: just allocate, no split
        if (temp_memory_block.segment_size == request_size) {
            memory_map[memory_map_index].process_id = process_id;
            allocated_memory = memory_map[memory_map_index];
            return allocated_memory;
        }

        // Split case
        allocated_memory.start_address = temp_memory_block.start_address;
        allocated_memory.end_address = allocated_memory.start_address + request_size - 1;
        allocated_memory.process_id = process_id;
        allocated_memory.segment_size = request_size;

        // Make room for new free block
        *map_cnt = *map_cnt + 1;
        for(int i = *map_cnt - 1; i > memory_map_index; i--){
            memory_map[i] = memory_map[i - 1];
        }

        // Put allocated block and remainder free block
        memory_map[memory_map_index] = allocated_memory;
        memory_map[memory_map_index + 1].start_address = allocated_memory.end_address + 1;
        memory_map[memory_map_index + 1].end_address = temp_memory_block.end_address;
        memory_map[memory_map_index + 1].segment_size = temp_memory_block.segment_size - request_size;
        memory_map[memory_map_index + 1].process_id = 0;
    }

    // If no match, return NULLBLOCK
    return allocated_memory;
}

/* -------------------- FIRST FIT -------------------- */

struct MEMORY_BLOCK first_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id){
    struct MEMORY_BLOCK temp_memory_block, allocated_memory;
    allocated_memory.start_address = 0;
    allocated_memory.end_address = 0;
    allocated_memory.segment_size = 0;
    allocated_memory.process_id = 0;  // NULLBLOCK

    int memory_map_index = -1;

    // Find first free block with size >= request_size
    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].process_id == 0 &&
            memory_map[i].segment_size >= request_size) {
            memory_map_index = i;
            break;
        }
    }

    if (memory_map_index == -1) {
        // No suitable block
        return allocated_memory;
    }

    temp_memory_block = memory_map[memory_map_index];

    // Exact fit: just allocate
    if (temp_memory_block.segment_size == request_size) {
        memory_map[memory_map_index].process_id = process_id;
        allocated_memory = memory_map[memory_map_index];
        return allocated_memory;
    }

    // Split case
    allocated_memory.start_address = temp_memory_block.start_address;
    allocated_memory.end_address = allocated_memory.start_address + request_size - 1;
    allocated_memory.segment_size = request_size;
    allocated_memory.process_id = process_id;

    // Make room for new free block
    *map_cnt = *map_cnt + 1;
    for (int i = *map_cnt - 1; i > memory_map_index; i--) {
        memory_map[i] = memory_map[i - 1];
    }

    // Place allocated and remaining free block
    memory_map[memory_map_index] = allocated_memory;
    memory_map[memory_map_index + 1].start_address = allocated_memory.end_address + 1;
    memory_map[memory_map_index + 1].end_address = temp_memory_block.end_address;
    memory_map[memory_map_index + 1].segment_size = temp_memory_block.segment_size - request_size;
    memory_map[memory_map_index + 1].process_id = 0;

    return allocated_memory;
}

/* -------------------- WORST (LARGEST) FIT -------------------- */

struct MEMORY_BLOCK worst_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id){
    struct MEMORY_BLOCK temp_memory_block, allocated_memory;
    allocated_memory.start_address = 0;
    allocated_memory.end_address = 0;
    allocated_memory.segment_size = 0;
    allocated_memory.process_id = 0;  // NULLBLOCK

    int memory_map_index = -1;
    int largest_size = 0;

    // Find largest free block with size >= request_size
    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].process_id == 0 &&
            memory_map[i].segment_size >= request_size) {

            if (memory_map_index == -1 || memory_map[i].segment_size > largest_size) {
                memory_map_index = i;
                largest_size = memory_map[i].segment_size;
            }
        }
    }

    if (memory_map_index == -1) {
        // No suitable block
        return allocated_memory;
    }

    temp_memory_block = memory_map[memory_map_index];

    // Exact fit
    if (temp_memory_block.segment_size == request_size) {
        memory_map[memory_map_index].process_id = process_id;
        allocated_memory = memory_map[memory_map_index];
        return allocated_memory;
    }

    // Split case
    allocated_memory.start_address = temp_memory_block.start_address;
    allocated_memory.end_address = allocated_memory.start_address + request_size - 1;
    allocated_memory.segment_size = request_size;
    allocated_memory.process_id = process_id;

    *map_cnt = *map_cnt + 1;
    for (int i = *map_cnt - 1; i > memory_map_index; i--) {
        memory_map[i] = memory_map[i - 1];
    }

    memory_map[memory_map_index] = allocated_memory;
    memory_map[memory_map_index + 1].start_address = allocated_memory.end_address + 1;
    memory_map[memory_map_index + 1].end_address = temp_memory_block.end_address;
    memory_map[memory_map_index + 1].segment_size = temp_memory_block.segment_size - request_size;
    memory_map[memory_map_index + 1].process_id = 0;

    return allocated_memory;
}

/* -------------------- NEXT FIT -------------------- */

struct MEMORY_BLOCK next_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id, int last_address){
    struct MEMORY_BLOCK temp_memory_block, allocated_memory;
    allocated_memory.start_address = 0;
    allocated_memory.end_address = 0;
    allocated_memory.segment_size = 0;
    allocated_memory.process_id = 0;  // NULLBLOCK

    // Find index to start search from: first block whose start_address >= last_address
    int start_index = 0;
    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].start_address >= last_address) {
            start_index = i;
            break;
        }
    }

    int memory_map_index = -1;

    // Search from start_index to end
    for (int i = start_index; i < *map_cnt; i++) {
        if (memory_map[i].process_id == 0 &&
            memory_map[i].segment_size >= request_size) {
            memory_map_index = i;
            break;
        }
    }

    // If not found, wrap around and search from 0 to start_index-1
    if (memory_map_index == -1) {
        for (int i = 0; i < start_index; i++) {
            if (memory_map[i].process_id == 0 &&
                memory_map[i].segment_size >= request_size) {
                memory_map_index = i;
                break;
            }
        }
    }

    if (memory_map_index == -1) {
        // No suitable block
        return allocated_memory;
    }

    temp_memory_block = memory_map[memory_map_index];

    // Exact fit
    if (temp_memory_block.segment_size == request_size) {
        memory_map[memory_map_index].process_id = process_id;
        allocated_memory = memory_map[memory_map_index];
        return allocated_memory;
    }

    // Split case
    allocated_memory.start_address = temp_memory_block.start_address;
    allocated_memory.end_address = allocated_memory.start_address + request_size - 1;
    allocated_memory.segment_size = request_size;
    allocated_memory.process_id = process_id;

    *map_cnt = *map_cnt + 1;
    for (int i = *map_cnt - 1; i > memory_map_index; i--) {
        memory_map[i] = memory_map[i - 1];
    }

    memory_map[memory_map_index] = allocated_memory;
    memory_map[memory_map_index + 1].start_address = allocated_memory.end_address + 1;
    memory_map[memory_map_index + 1].end_address = temp_memory_block.end_address;
    memory_map[memory_map_index + 1].segment_size = temp_memory_block.segment_size - request_size;
    memory_map[memory_map_index + 1].process_id = 0;

    return allocated_memory;
}

/* -------------------- RELEASE MEMORY -------------------- */

void release_memory(struct MEMORY_BLOCK freed_block, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt){
    int idx = -1;

    // Find the block to be freed (match by start/end/size)
    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].start_address == freed_block.start_address &&
            memory_map[i].end_address == freed_block.end_address &&
            memory_map[i].segment_size == freed_block.segment_size) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        // Block not found; nothing to do
        return;
    }

    // Mark as free
    memory_map[idx].process_id = 0;

    // Merge with previous free block if possible
    if (idx > 0 && memory_map[idx - 1].process_id == 0) {
        memory_map[idx - 1].end_address = memory_map[idx].end_address;
        memory_map[idx - 1].segment_size += memory_map[idx].segment_size;

        // Shift everything left from idx onward
        for (int j = idx; j < *map_cnt - 1; j++) {
            memory_map[j] = memory_map[j + 1];
        }
        (*map_cnt)--;
        idx--;  // merged block is now at idx-1
    }

    // Merge with next free block if possible
    if (idx < *map_cnt - 1 && memory_map[idx + 1].process_id == 0) {
        memory_map[idx].end_address = memory_map[idx + 1].end_address;
        memory_map[idx].segment_size += memory_map[idx + 1].segment_size;

        for (int j = idx + 1; j < *map_cnt - 1; j++) {
            memory_map[j] = memory_map[j + 1];
        }
        (*map_cnt)--;
    }
}
