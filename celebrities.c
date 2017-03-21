#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "merge.h"

int merge_sort_join(int total_mem, int block_size);

int main (int argc, char *argv[]){


  if (argc != 2){
    printf ("Usage: celebrity <file_name> \n");
    exit(1);
  }

  // Hard coded total memory size and optimal block size
  static const int memory = 209715200;
  static const int  block_size = 1048576;

    
  // int block_size = atoi(atgv[2]);
  disk_sort(argv[1], memory, block_size, "UID1", "UID1_sorted_merge.dat");
  disk_sort(argv[1], memory, block_size, "UID2", "UID2_sorted_merge.dat");
  write_degree("UID1_sorted_merge.dat", block_size, 0, "outdegree.dat");
  write_degree("UID2_sorted_merge.dat", block_size, 1, "indegree.dat");
  merge_sort_join(memory, block_size);
  disk_sort_celeb("celeb.dat", memory, block_size, "sorted_celeb.dat");
   
  FILE *fp_read;
  if (!(fp_read = fopen ( "sorted_celeb.dat", "rb" ))){
	   return -1;
	}
	CelebritiesRecord * buffer = (CelebritiesRecord *) calloc (block_size, sizeof (CelebritiesRecord));

	int r = fread (buffer, sizeof(CelebritiesRecord), block_size, fp_read);
  if(r == 0){
    return -1;
  }
	int count= 0;
	while (count<10){
	   printf("uid: %d , in-out: %d, indegree: %d, outdegree: %d\n",buffer[count].UID1,buffer[count].count, buffer[count].indegree, buffer[count].outdegree);
	   count++;
  }
  fclose (fp_read);
  free (buffer);
  remove("celeb.dat");
  remove("UID2_sorted_merge.dat");
  remove("UID1_sorted_merge.dat");
  remove("outdegree.dat");
  remove("indegree.dat");
  return 0;
   
}

int merge_sort_join(int total_mem, int block_size){
  
    NewMergeManager * manager = (NewMergeManager *)calloc(1, sizeof(NewMergeManager));
    int records_per_block  = block_size/sizeof(Record);
    int memory_per_block = total_mem/block_size;
    int records_per_buffer = (memory_per_block / 3)*records_per_block * sizeof(Record);
    int Celebrities_records_per_block = block_size/sizeof(CelebritiesRecord);
    int Celebrities_records_per_buffer = (memory_per_block / 3)*Celebrities_records_per_block*sizeof(CelebritiesRecord);

    manager->input_buffer_capacity = records_per_buffer;
    manager->output_buffer_capacity = Celebrities_records_per_buffer;
    int current_input_file_positions[2];
    int current_input_buffer_positions[2];
    int total_input_buffer_elements[2];
    Record** input_buffers = malloc(2 * sizeof(Record *));
    strcpy(manager->output_file_name , "celeb.dat");
    strcpy(manager->input_file_name_1, "outdegree.dat");
    strcpy(manager->input_file_name_2, "indegree.dat");
    int i;

    for(i = 0; i < 2; i++){
        total_input_buffer_elements[i] = 0;

        current_input_file_positions[i] = 0;
        current_input_buffer_positions[i] = 0;

        input_buffers[i] = (Record *)calloc(manager->input_buffer_capacity, sizeof(Record));
    }   
    manager->output_buffer_Celebrities = (CelebritiesRecord *)calloc(manager->output_buffer_capacity, sizeof(CelebritiesRecord));
    manager->current_output_buffer_position = 0;
    manager->input_buffers = input_buffers;
    manager->current_input_file_positions = current_input_file_positions;
    manager->current_input_buffer_positions = current_input_buffer_positions;
    manager->total_input_buffer_elements = total_input_buffer_elements;
    manager->is_query_true_friends = 1;
    new_merge_runs(manager);
    return 0;

}
