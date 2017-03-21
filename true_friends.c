#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "merge.h"

int main (int argc, char *argv[]){
  
  
  if (argc != 2){
    printf ("Usage: true_friends <file_name> \n");
    exit(1);
  }
  
  // Hard coded total memory size and optimal block size
  static const int memory = 209715200;
  static const int block_size = 1048576;
  
  char* input_file = argv[1];
  disk_sort(input_file, memory, block_size, "UID1", "UID1_sorted_merge");
  disk_sort(input_file, memory, block_size, "UID2", "UID2_sorted_merge");
  
  // to do : scan sorted file uid1 and uid2, and calculate true friends;
  sorted_merge_join(memory, block_size);
  //remove("UID1_sorted_merge.dat");
  //remove("UID2_sorted_merge.dat");
  return 0;
}


int sorted_merge_join(int total_mem, int block_size){
  
  NewMergeManager * manager = (NewMergeManager *)calloc(1, sizeof(NewMergeManager));
  int records_per_block  = block_size/sizeof(Record);
  int mem_per_block = total_mem/block_size;
  int records_per_buffer = (mem_per_block / 3)*records_per_block * sizeof(Record);
  
  manager->input_buffer_capacity = records_per_buffer;
  manager->output_buffer_capacity = records_per_buffer + (mem_per_block%3)*records_per_block * sizeof(Record);
  int current_input_file_positions[2];
  int current_input_buffer_positions[2];
  int total_input_buffer_elements[2];
  Record** input_buffers = malloc(2 * sizeof(Record *));
  strcpy(manager->output_file_name , "true_friends.dat");
  strcpy(manager->input_file_name_1, "UID1_sorted_merge.dat");
  strcpy(manager->input_file_name_2, "UID2_sorted_merge.dat");
  int i;
  
  for(i = 0; i < 2; i++){
    total_input_buffer_elements[i] = 0;
    
    current_input_file_positions[i] = 0;
    current_input_buffer_positions[i] = 0;
    
    input_buffers[i] = (Record *)calloc(manager->input_buffer_capacity, sizeof(Record));
  }   
  manager->output_buffer = (Record *)calloc(manager->output_buffer_capacity, sizeof(Record));
  manager->input_buffers = input_buffers;
  manager->current_output_buffer_position = 0;
  manager->current_input_file_positions = current_input_file_positions;
  manager->current_input_buffer_positions = current_input_buffer_positions;
  manager->total_input_buffer_elements = total_input_buffer_elements;
  manager->is_query_true_friends = 0;
  new_merge_runs(manager);
  return 0;
  
}

