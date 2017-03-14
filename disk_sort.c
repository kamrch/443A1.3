#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include "merge.h"

/**
* Compares two records a and b
* with respect to the value of the integer field f.
* Returns an integer which indicates relative order:
* positive: record a > record b
* negative: record a < record b
* zero: equal records
*/

//taken from handout
int compare (const void *a, const void *b) {
    int a_f = ((const struct record*) a)->UID2;
    int b_f = ((const struct record*) b)->UID2;
    return (a_f - b_f);
}


//Sort function that uses qsort, some codes are from handout
void sort(Record* buffer, int total_records){
    qsort (buffer, total_records, sizeof(Record), compare);
}

// function to print buffer contents to stdout
void print_buffer(Record* buffer, int total_records){
    printf("=====Total records: %d\n=====", total_records);
    for (int i=0; i<total_records; i++){
        printf("uid2 = %i, uid1 = %i\n", buffer[i].UID2, buffer[i].UID1);
    }
}

//function for initializing the manager to pass to merge_run
int merge_runs_init(int block_size, int total_mem, int buffer_num) {
    MergeManager * manager = (MergeManager *)calloc(1, sizeof(MergeManager));
    //set up attributes of MergeManager shown in merge.h
    int records_per_block = block_size / sizeof(Record);
    int block_num = total_mem / block_size;
    int blocks_per_buffer = block_num / (buffer_num+1);
    int records_per_buffer = records_per_block * blocks_per_buffer;

    manager->heap_capacity = buffer_num;
    manager->heap = (HeapElement *)calloc(buffer_num, sizeof(HeapElement));
    strcpy(manager->input_prefix, "output");
    strcpy(manager->output_file_name , "sorted_merge.dat");
    if (block_num % (buffer_num+1) > 0){
        int remaining_block = block_num % (buffer_num+1);
        manager->output_buffer_capacity = records_per_buffer + records_per_block * remaining_block;
    }
    else {
        manager->output_buffer_capacity = records_per_buffer;
    }
    manager->input_buffer_capacity = records_per_buffer;

    int input_file_numbers[buffer_num];
    int current_file_positions[buffer_num];
    int current_buffer_positions[buffer_num];
    int total_input_buffer_elements[buffer_num];
    Record** input_buffers = (Record**) malloc(buffer_num * sizeof(Record *));
    int i;
    for(i = 0; i < buffer_num; i++){
        input_file_numbers[i] = i;
        current_file_positions[i] = 0;
        current_buffer_positions[i] = 0;
        total_input_buffer_elements[i] = 0;
        input_buffers[i] = (Record *)calloc(records_per_buffer, sizeof(Record));
    }
    manager->input_file_numbers = input_file_numbers;
    manager->output_buffer = (Record *)calloc(manager->output_buffer_capacity, sizeof(Record));
    manager->current_output_buffer_position = 0;
    manager->input_buffers = input_buffers;
    manager->current_input_file_positions = current_file_positions;
    manager->current_input_buffer_positions = current_buffer_positions;
    manager->total_input_buffer_elements = total_input_buffer_elements;
    // calls merge_runs in merge_external.c
    merge_runs(manager);
    return 0;
}

// function for Phase 1 of 2PMMS
int main(int argc, char *argv[]){

    if (argc != 4){
        printf ("Usage: disk_sort <file_name> <total_memory> <block_size>\n");
        exit(1);
    }

    char *input_file = argv[1];
    int total_mem = atoi(argv[2]);
    int block_size = atoi(argv[3]);
    FILE *fp_read;

    //check if enough memory
    if(block_size > total_mem){
        printf("Error: Block size is larger than total memory.\n");
        exit(0);
    }
    int block_num = (total_mem / block_size);

    if (!(fp_read= fopen (input_file , "rb" ))) {
        printf ("Error when reading file \"%s\"\n", input_file);
        exit(0);
    }

    /*finding the file size and total number of records*/
    fseek(fp_read, 0, SEEK_END);
    int file_size = ftell(fp_read);
    // int total_records = file_size / sizeof(Record);
    //find the number of chunks
    int chunks = file_size/(block_num * block_size);
    int records_per_block = block_size / sizeof(Record);
    int remaining_chunk = file_size - (chunks * block_num * block_size);
    //find the number of records in each chunk
    int chunk_records = records_per_block * block_num;
    int remaining_chunk_records = remaining_chunk / sizeof(Record);

    int sublist = chunks;
    if (remaining_chunk == 0){
        sublist++;
    }

    if ((sublist+1) > total_mem/block_size){
        perror("Error: not enough memory allocated.");
        exit(1);
    }

    //reset pointer location
    fseek(fp_read, 0, SEEK_SET);
    int i = 0;

    while (i < chunks+1) {
        FILE *fp_write;
        char output_file[MAX_PATH_LENGTH];
        snprintf(output_file, sizeof(char) * MAX_PATH_LENGTH, "output%d.dat", i);
        if (!(fp_write = fopen ( output_file , "wb" ))) {
            printf ("Error when writing file sorted_list  \n");
            exit(1);
        }
        if (i == chunks){
            if(remaining_chunk != 0){
                Record * buffer = (Record *) calloc (remaining_chunk_records, sizeof (Record));
                if (fread (buffer, sizeof(Record), remaining_chunk_records, fp_read) == 0){
                    perror("Error: Failed reading buffer.\n");
                }
                qsort (buffer, remaining_chunk_records, sizeof(Record), compare);
                fwrite(buffer, sizeof(Record), remaining_chunk_records, fp_write);
                //print_buffer(buffer, total_records);
                fflush (fp_write);
                free (buffer);
            } else {
                break;
            }

        } else {
            Record * buffer = (Record *) calloc (chunk_records, sizeof (Record));
            if (fread (buffer, sizeof(Record), chunk_records, fp_read) == 0){
                perror("Error: Failed reading buffer.\n");
            }
            qsort (buffer, chunk_records, sizeof(Record), compare);
            fwrite(buffer, sizeof(Record), chunk_records, fp_write);
            //print_buffer(buffer, total_records);
            fflush (fp_write);
            free(buffer);
        }
        //free(output_file);
        fclose(fp_write);
        i++;

    }
    fclose(fp_read);
    //merge here
    merge_runs_init(block_size, total_mem, sublist+1);
    return 0;
}
