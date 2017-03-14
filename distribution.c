#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merge.h"

int main(int argc, char **argv){
	if (argc != 5){
		printf ("Usage: distribution <file_name> <block_size> <column_id> <max_degree>");
        exit(1);
	}

	char *file_name = argv[1];
    int block_size = atoi(argv[2]);
    char* column_id = argv[3];
    int max_degree = atoi(argv[4]);
    int results[max_degree+1];

    /* initialize the result array */
    for (int z=0; z<max_degree+1; z++){
        results[z] = 0; 
    }

    int records_per_block = block_size / sizeof(Record);
    //Record * buffer = (Record * ) calloc(records_per_block, sizeof(Record));
    Record * buffer = calloc(records_per_block, sizeof(Record));

    FILE *fp_read;
    if (!(fp_read = fopen (file_name , "rb"))){
        perror("Error in distribution.c: fp_read fopen error.\n");
        exit(1);
    }


	FILE *fp_write;
	char output_file_name[20] = "merged";
	if (strcmp(column_id, "UID1") == 0){
		// case UID1
		strcat(output_file_name, "1");
	} else if (strcmp(column_id, "UID2") == 0) {
        // case UID2
        strcat(output_file_name, "2");
    } else {
        // incorrect input case
        printf("Error: <column_id> must be only either 'UID1' or 'UID2'.\n");
        exit(1);
    }

    strcat(output_file_name, ".dat");
    // printf("%s\n", output_file_name);
    if (!(fp_write = fopen(output_file_name, "wb"))) {
        printf("Error in distribution.c: fp_write fopen error.  \n");
        exit(1);
    }

    int counter = 0;
    int curr_id = -1;
    int read_records = 0;


    while ((read_records = fread(buffer, sizeof(Record), records_per_block, fp_read)) > 0) {
        int i;
        records_per_block = block_size / sizeof(Record);

        //Check if total number of records read from the file is less than 1 block
        if (records_per_block != read_records){
            records_per_block = read_records;
        }

        for (i = 0; i < records_per_block; i++){
            //init case
            if (curr_id == -1){
                if (strcmp(column_id, "UID1")==0){
                    curr_id = buffer[i].UID1;
                } else {
                    curr_id = buffer[i].UID2;
                }
            }


            if (strcmp(column_id, "UID1")==0 && curr_id != buffer[i].UID1){
                results[counter]++;
                counter = 0;
                curr_id = buffer[i].UID1;
            } else if (strcmp(column_id, "UID2")==0 && curr_id != buffer[i].UID2){
                results[counter]++;
                counter = 0;
                curr_id = buffer[i].UID2;
            }

            counter ++;
        }
    }
    results[counter]++;
    //writing results to output file
    for (int z = 0; z <= max_degree; z++){
        if (results[z] != 0){
            fprintf(fp_write, "%d, %d\n", z, results[z]);            
        }        
    }
    fclose(fp_write);
    fclose(fp_read);
    free(buffer);
    return 0;
}
