#include "oslabs.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

struct MEMORY_BLOCK best_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id){
    // changed allo_mem -> allocated_memory
    struct MEMORY_BLOCK temp_memory_block, allocated_memory;
    allocated_memory.end_address = 0;
    allocated_memory.start_address = 0;
    allocated_memory.process_id = 0;
    allocated_memory.segment_size = 0; 
    bool match = false;
    int memory_map_index = 0, best_fit_segment = 0;

    //find the candidate memory blocks that can be allocated and chooses the one whose size is closest to the requested size
    for(int i = 0; i <= *map_cnt - 1; i++){
        if(request_size == memory_map[i].segment_size && memory_map[i].process_id == 0){
            match = true;
            memory_map_index = i;
            best_fit_segment = request_size;
            break;
        }
        else if((memory_map[i].segment_size > request_size) && (memory_map[i].process_id == 0)) {
            if(match == false){
                match = true;
                memory_map_index = i;
                best_fit_segment = memory_map[i].segment_size;
            }
            else{
                if(best_fit_segment > memory_map[i].segment_size){
                    memory_map_index = i;
                    best_fit_segment = memory_map[i].segment_size;
                }
            }
        }
    }

    if (match == true){
        temp_memory_block = memory_map[memory_map_index];

        if (temp_memory_block.segment_size == request_size) {
            memory_map[memory_map_index].process_id = process_id;
            allocated_memory = memory_map[memory_map_index];
            return allocated_memory;
        }

        // Otherwise, split the block
        allocated_memory.start_address = temp_memory_block.start_address;
        allocated_memory.end_address = allocated_memory.start_address + request_size - 1;
        allocated_memory.process_id = process_id;
        allocated_memory.segment_size = request_size;

        // Increase map count first
        *map_cnt = *map_cnt + 1;

        for(int i = *map_cnt - 1; i > memory_map_index; i--){
            memory_map[i] = memory_map[i - 1];
        }

        // Now create the remaining free block at memory_map_index + 1
        memory_map[memory_map_index] = allocated_memory;
        memory_map[memory_map_index + 1].start_address = allocated_memory.end_address + 1;
        memory_map[memory_map_index + 1].end_address = temp_memory_block.end_address;
        memory_map[memory_map_index + 1].process_id = 0;
        memory_map[memory_map_index + 1].segment_size = temp_memory_block.segment_size - request_size;
    }

    // If match == false, allocated_memory is still {0,0,0,0} (NULLBLOCK)
    return allocated_memory;
}
