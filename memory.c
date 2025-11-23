#include "oslabs.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

struct MEMORY_BLOCK best_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id){
    struct MEMORY_BLOCK temp_memory_block, allo_memory;
    allo_mem.end_address = 0;
    allo_mem.start_address = 0;
    allo_mem.process_id = 0;
    allo_mem.segment_size = 0; 
    bool match = false;
    int memory_map_index = 0, best_fit_segment = 0;

    //find the candidate memory blocks that can be allocated and chooses the one whose size is closest to the requested size
    for(int i = 0; i <= *map_cnt - 1; i++){
      if(req_size == memory_map[i].segment_size){
        match = true;
        memory_map_index = i;
        best_fit_segment = request_size;
        break;
      }
      else if((memory_map[i].seg_size > req_size)&&(memory_map[i].process_id == 0)) {
        if(match==false){
          match=true;
          memory_map_index = i;
          best_fit_segment = memory_map[i].segment_size)}
        else{
          if(best_fit_segment > memory_map[i].segment_size){
            memory_map_index = i;
            best_fit_segment = memory_map[i].segment_size;
          }
        }
      }
    }
}
