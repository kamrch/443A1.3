#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "merge.h"
// #include "disk.h"


int compare_celebrity(const void *a, const void *b) {
    int a_count = ((const struct celebrities_record*)a)->count;
    int b_count = ((const struct celebrities_record*)b)->count;
    if(a_count < b_count){
        return 1;
    }
    return -1;
}

int disk_sort_q2(char* filename, int total_mem, int block_size, char* output_filename){

 	FILE *fp_read;
    
    int block_num= (total_mem / block_size);

	if (!(fp_read = fopen (filename , "rb" ))){
		printf ("Error when reading file \"%s\"\n", filename);
        exit(0);
	}	

	// find file size
    fseek(fp_read, 0L, SEEK_END);
	int file_size = ftell(fp_read);
    int records_per_block = block_size / sizeof(CelebritiesRecord);
	int chunk_num = file_size/total_mem;
    int last_chunk_size = file_size - chunk_num*total_mem;
    int records_per_chunk = records_per_block*block_num;
    int records_last_chunk = last_chunk_size / sizeof(CelebritiesRecord);
    int num_sublist;
    if (last_chunk_size!=0){
    	num_sublist = chunk_num;

    }else{
        num_sublist = chunk_num + 1; 
    }
    if ((num_sublist+1) > total_mem/block_size){
    	perror("Error: not enough memory allocated.");
        exit(1);
    }

    //reset pointer location
	fseek(fp_read, 0L, SEEK_SET);
    int i = 0;


    while (i < chunk_num+1){
    	FILE *fp_write;
    	char k[chunk_num+1];
		sprintf(k,"%d",i);

        char filename[MAX_PATH_LENGTH];
        // output file name construction
        snprintf(filename, sizeof(char) * MAX_PATH_LENGTH, "sorted3_%s.dat", k);

		// char * filename = (char *) calloc(20+chunk_num+1, sizeof(char));
  //       strcat(filename,"sorted3_");
		// strcat(filename,k);        
		// strcat(filename,".dat");

		fp_write = fopen(filename, "wb");
		if(fp_write == NULL){
	 	     perror("Error opening file");
	 	     return -1;
	    }	
        if (i == chunk_num) {
        	if (last_chunk_size== 0){
                   break;

        	}else{
        		CelebritiesRecord * buffer = (CelebritiesRecord *) calloc (records_last_chunk, sizeof (CelebritiesRecord));        	
                if (fread (buffer, sizeof(CelebritiesRecord), records_last_chunk, fp_read);==0){
                    perror("Error: Failed reading buffer.\n");
                }

                qsort (buffer, records_last_chunk, sizeof(CelebritiesRecord), compare_celebrity);
                

				fwrite(buffer, sizeof(CelebritiesRecord), records_last_chunk, fp_write);
				fflush (fp_write);
                free (buffer);
        	}            
        } else{
            CelebritiesRecord * buffer = (CelebritiesRecord *) calloc (records_per_chunk, sizeof (CelebritiesRecord));

            if (fread (buffer, sizeof(CelebritiesRecord), records_per_chunk, fp_read)==0){
                perror("read buffer failed\n");
            }

        qsort (buffer, records_per_chunk, sizeof(CelebritiesRecord), compare_celebrity);
        
		fwrite(buffer, sizeof(CelebritiesRecord), records_per_chunk, fp_write);
		fflush (fp_write);

		free(buffer);
		
	   }
	   free(filename);
	   fclose(fp_write);

	   i++; 

   }
   fclose(fp_read);
   celebrity_merge_sort(num_sublist+1, total_mem, block_size, output_filename);
   return 0;
   
 }

 int celebrity_merge_sort(int buffer_num, int total_mem, int block_size, char* output_filename){

 	Q2MergeManager * manager = (Q2MergeManager *)calloc(1, sizeof(Q2MergeManager));
 
 	int records_per_block  = block_size / sizeof(CelebritiesRecord);
 	int block_num = total_mem/block_size;
 	int records_per_buffer = records_per_block * (block_num / (buffer_num + 1));

 	manager->heap_capacity = buffer_num;
 	manager->heap = (Q2HeapElement *)calloc(buffer_num, sizeof(Q2HeapElement));
 	strcpy(manager->output_file_name , output_filename);

    strcpy(manager->input_prefix, "sorted3_"); 
 	
 	if(block_num % (buffer_num + 1) > 0){
 		int extra_block = block_num % (buffer_num + 1);
 		manager->output_buffer_capacity = records_per_buffer + extra_block * records_per_block;
 	}else{
 		manager->output_buffer_capacity = records_per_buffer;
 	}
 	
 	manager->input_buffer_capacity = records_per_buffer;
 	
 	int input_file_numbers[buffer_num];
 	int current_file_positions[buffer_num];
 	int current_buffer_positions[buffer_num];
 	int total_input_buffer_elements[buffer_num];
 	CelebritiesRecord** input_buffers = malloc(buffer_num * sizeof(CelebritiesRecord *));
 	int i;
 	for(i = 0; i < manager->heap_capacity; i++){
 		input_file_numbers[i] = i;
 		current_file_positions[i] = 0;
 		current_buffer_positions[i] = 0;
 		total_input_buffer_elements[i] = 0;
 		input_buffers[i] = (CelebritiesRecord *)calloc(manager->input_buffer_capacity, sizeof(CelebritiesRecord));
 	}	
 	manager->input_file_numbers = input_file_numbers;
 	manager->output_buffer = (CelebritiesRecord *)calloc(manager->output_buffer_capacity, sizeof(CelebritiesRecord));
 	manager->current_output_buffer_position = 0;
 	manager->input_buffers = input_buffers;
 	manager->current_input_file_positions = current_file_positions;
 	manager->current_input_buffer_positions = current_buffer_positions;
 	manager->total_input_buffer_elements = total_input_buffer_elements;
 	q2_merge_runs(manager);
 	return 0;
 }
