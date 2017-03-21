#include "merge.h"

//manager fields should be already initialized in the caller
int merge_runs (MergeManager * merger){
	int  result; //stores SUCCESS/FAILURE returned at the end	

	//1. go in the loop through all input files and fill-in initial buffers
	if (init_merge (merger)!=SUCCESS)
		return FAILURE;

	while (merger->current_heap_size > 0) { //heap is not empty
		HeapElement smallest;
		Record next; //here next comes from input buffer
		
		if(get_top_heap_element (merger, &smallest)!=SUCCESS)
			return FAILURE;

		result = get_next_input_element (merger, smallest.run_id, &next);
		
		if (result==FAILURE)
			return FAILURE;

		if(result==SUCCESS) {//next element exists, may also return EMPTY
			if(insert_into_heap (merger, smallest.run_id, &next)!=SUCCESS)
				return FAILURE;
		}		


		merger->output_buffer [merger->current_output_buffer_position].UID1=smallest.UID1;
		merger->output_buffer [merger->current_output_buffer_position].UID2=smallest.UID2;
		
		merger->current_output_buffer_position++;

        //staying on the last slot of the output buffer - next will cause overflow
		if(merger->current_output_buffer_position == merger-> output_buffer_capacity ) {
			if(flush_output_buffer(merger)!=SUCCESS) {
				return FAILURE;			
				merger->current_output_buffer_position=0;
			}	
		}
	
	}

	
	//flush what remains in output buffer
	if(merger->current_output_buffer_position > 0) {
		if(flush_output_buffer(merger)!=SUCCESS)
			return FAILURE;
	}
	
	clean_up(merger);
	return SUCCESS;	
}


int get_top_heap_element (MergeManager * merger, HeapElement * result){
	HeapElement item;
	int child, parent;

	if(merger->current_heap_size == 0){
		printf( "UNEXPECTED ERROR: popping top element from an empty heap\n");
		return FAILURE;
	}

	*result=merger->heap[0];  //to be returned

	//now we need to reorganize heap - keep the smallest on top
	item = merger->heap [--merger->current_heap_size]; // to be reinserted 

	parent =0;
	if (strcmp(merger->sorted_uid, "UID1") == 0){
		while ((child = (2 * parent) + 1) < merger->current_heap_size) {
			// if there are two children, compare them 
			if (child + 1 < merger->current_heap_size && 
					(compare_heap_elements_UID1(&(merger->heap[child]),&(merger->heap[child + 1]))>0)) 
				++child;
			
			// compare item with the larger 
			if (compare_heap_elements_UID1(&item, &(merger->heap[child]))>0) {
				merger->heap[parent] = merger->heap[child];
				parent = child;
			} 
			else 
				break;
		}
	}
	else if (strcmp(merger->sorted_uid, "UID2") == 0){
		while ((child = (2 * parent) + 1) < merger->current_heap_size) {
			// if there are two children, compare them 
			if (child + 1 < merger->current_heap_size && 
					(compare_heap_elements_UID2(&(merger->heap[child]),&(merger->heap[child + 1]))>0)) 
				++child;
			
			// compare item with the larger 
			if (compare_heap_elements_UID2(&item, &(merger->heap[child]))>0) {
				merger->heap[parent] = merger->heap[child];
				parent = child;
			} 
			else 
				break;
		}
	}
	else{
		while ((child = (2 * parent) + 1) < merger->current_heap_size) {
			// if there are two children, compare them 
			if (child + 1 < merger->current_heap_size && 
					(compare_heap_elements(&(merger->heap[child]),&(merger->heap[child + 1]))>0)) 
				++child;
			
			// compare item with the larger 
			if (compare_heap_elements(&item, &(merger->heap[child]))>0) {
				merger->heap[parent] = merger->heap[child];
				parent = child;
			} 
			else 
				break;
		}
	}
	
	merger->heap[parent] = item;
	
	return SUCCESS;
}

int insert_into_heap (MergeManager * merger, int run_id, Record *input){

	HeapElement new_heap_element;
	int child, parent;

	new_heap_element.UID1 = input->UID1;
	new_heap_element.UID2 = input->UID2;
	new_heap_element.run_id = run_id;
	
	if (merger->current_heap_size == merger->heap_capacity) {
		printf( "Unexpected ERROR: heap is full\n");
		return FAILURE;
	}
  	
	child = merger->current_heap_size++; /* the next available slot in the heap */
	
	if (strcmp(merger->sorted_uid, "UID1") == 0){
		while (child > 0) {
			parent = (child - 1) / 2;
			if (compare_heap_elements_UID1(&(merger->heap[parent]),&new_heap_element)>0) {
				merger->heap[child] = merger->heap[parent];
				child = parent;
			} 
			else 
				break;
		}
	}
	else if (strcmp(merger->sorted_uid, "UID2") == 0){
		while (child > 0) {
			parent = (child - 1) / 2;
			if (compare_heap_elements_UID2(&(merger->heap[parent]),&new_heap_element)>0) {
				merger->heap[child] = merger->heap[parent];
				child = parent;
			} 
			else 
				break;
		}
	}
	else{
		while (child > 0) {
			parent = (child - 1) / 2;
			if (compare_heap_elements(&(merger->heap[parent]),&new_heap_element)>0) {
				merger->heap[child] = merger->heap[parent];
				child = parent;
			} 
			else 
				break;
		}
	}
	merger->heap[child]= new_heap_element;	
	return SUCCESS;
}


/*
** TO IMPLEMENT
*/

int init_merge (MergeManager * manager) {
  /*
   * The merge starts with pre-filling of input buffer arrays with records from each run.
   * The suggestion is to add the heads of each array to a heap data structure, and then remove an element from the top of the heap,
   * transfer it to the output buffer, and insert into the heap the next element from the same run as the element being transferred.
   */
  // printf("hiasdsad\n");
  FILE *fp;
  for(int n = 0; n < manager->heap_capacity; n++){
    // output file name construction
    char file_number[20];
    sprintf(file_number,"%d",manager->input_file_numbers[n]);
    char output_file[MAX_PATH_LENGTH];
    snprintf(output_file, sizeof(char) * MAX_PATH_LENGTH, "%s%s.dat", manager->input_prefix, file_number);
    
    if ((fp = fopen (output_file , "rb" ))){
      // printf ("fopen success\n");
      fseek(fp, manager->current_input_file_positions[n]*sizeof(Record), SEEK_SET);
      int result = fread (manager->input_buffers[n], sizeof(Record), manager->input_buffer_capacity, fp);
      if (result <= 0) {
	manager->current_heap_size -= 1;
      }
      manager->total_input_buffer_elements[n] = result;
      manager->current_input_file_positions[n] =  result;
      insert_into_heap(manager, manager->input_file_numbers[n], &manager->input_buffers[n][manager->current_input_buffer_positions[n]]);
      manager->current_input_buffer_positions[n]++;
      
    } else {
      // printf ("here 11111\n");
      
      return FAILURE;
    }
    // printf ("here 22222\n");
    fclose(fp);
    
  }
  
  return SUCCESS;
}

int flush_output_buffer (MergeManager * manager) {
	FILE *fp_write;
	if (!(fp_write = fopen (manager->output_file_name , "a" ))){
		fprintf(stderr, "Error when flushing output buffer.\n");
		return FAILURE;
	}

	fwrite(manager->output_buffer, sizeof(Record), manager->current_output_buffer_position, fp_write);
	fflush (fp_write);
	fclose(fp_write);

	// Reset the pointer location
	manager->current_output_buffer_position = 0;

	return SUCCESS;
}

int get_next_input_element(MergeManager * manager, int file_number, Record *result) {

	if(manager->total_input_buffer_elements[file_number] == manager->current_input_buffer_positions[file_number]){
		manager->current_input_buffer_positions[file_number] = 0;

        // non SUCCESSFUL CASE
		if(refill_buffer (manager, file_number)!=0){
			return FAILURE;
		}
		
		if(manager->current_input_file_positions[file_number] == -1){
			return EMPTY;
		}

	}


	*result = manager->input_buffers[file_number][manager->current_input_buffer_positions[file_number]];
	manager->current_input_buffer_positions[file_number]+=1;

	return SUCCESS;
}

int refill_buffer (MergeManager * manager, int file_number) {
	FILE *fp;
	char num[20];
    sprintf(num,"%d",manager->input_file_numbers[file_number]);
    char output_file[MAX_PATH_LENGTH];
    snprintf(output_file, sizeof(char) * MAX_PATH_LENGTH, "%s%s.dat", manager->input_prefix, num);

	if ((fp = fopen (output_file, "rb" ))){
          fseek(fp, manager->current_input_file_positions[file_number]*sizeof(Record), SEEK_SET);
          int result = fread (manager->input_buffers[file_number], sizeof(Record), manager->input_buffer_capacity, fp);
          if(result > 0){
              manager->total_input_buffer_elements[file_number] = result;
              manager->current_input_file_positions[file_number]+= result;
          }else{
              manager->current_input_file_positions[file_number] = -1;
          }
        }else{
    	// printf ("here 333333\n");
          return FAILURE;
        }



	// printf ("here 444444\n");
	fclose(fp);

	return SUCCESS;
}

void clean_up (MergeManager * merger) {
	free(merger->heap);
	int i;
	for(i = 0; i < merger->heap_capacity; i++){
		free(merger->input_buffers[i]);
	}
	for(i = 0; i < merger->heap_capacity; i++){
		char k[100];
		char * filename = (char *) calloc(121,sizeof(char));
		sprintf(k,"%d",merger->input_file_numbers[i]);
		strcat(filename,merger->input_prefix);
		strcat(filename,k);
		strcat(filename,".dat");
		remove(filename);
		free(filename);
	}
	free(merger->input_buffers);
	free(merger->output_buffer);
	free(merger);
	
}

int compare_heap_elements (HeapElement *a, HeapElement *b) {
	if (a->UID2>b->UID2){
	//	if ((a->UID2-b->UID2)>0){output_
		return 1;
	} 
	// else if ((a->UID2==b->UID2) && (a->UID1>b->UID1)){
 //        return 1;
 //    }
	return 0;
}

int compare_heap_elements_UID1 (HeapElement *a, HeapElement *b){
  if (a->UID1 > b->UID1){
    return 1;
  }
  else if (a->UID1 == b->UID1 && a->UID2 > b->UID2){
    return 1;
  }
  return 0;
}

int compare_heap_elements_UID2 (HeapElement *a, HeapElement *b){
  if (a->UID2 > b->UID2){
    return 1;
  }
  else if (a->UID2 == b->UID2 && a->UID1 > b->UID1){
    return 1;
  }
  return 0;
}
