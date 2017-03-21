#include "merge.h"

//manager fields should be already initialized in the caller
int new_merge_runs (NewMergeManager * merger){	
  
  //1. go in the loop through all input files and fill-in initial buffers
  if (new_init_merge (merger)!=SUCCESS)
    return FAILURE;
  
  int counter = 0;
  Record* left = (Record*)calloc(1, sizeof(Record));
  Record* right = (Record*)calloc(1, sizeof(Record));
  
  if (merger->is_query_true_friends == 0){
    join_true_friends(left, right, merger, &counter);
  }
  else{ // query_Celebrities
    join_celebrities(left, right, merger, &counter);
  }
  
  free(left);
  free(right);
  
   //flush what remains in output buffer
  if(merger->current_output_buffer_position > 0) {
    if(new_flush_output_buffer(merger)!=SUCCESS)
      return FAILURE;
  }
  
  if (merger->is_query_true_friends == 0){
    printf("Total number of true friends is %d\n", counter);
  }
  else{ // query_Celebrities
    printf("Total number of distinct user is %d\n", counter);
  }
  
  new_clean_up(merger);
  return SUCCESS;	
}


int join_true_friends(Record *left, Record *right, NewMergeManager * merger, int *counter){
  //stores SUCCESS/FAILURE returned at the end
  int result1; 	
  int result2;
  while (merger->current_input_file_positions[0] != -1 && merger->current_input_file_positions[1] != -1){
    if(merger->current_input_file_positions[0] != -1){
      result1 = new_get_next_input_element(merger, 0, left);
    }else{
      result1 = EMPTY;
    }
    
    if (result1==FAILURE){ 
      return FAILURE;
    }
    
    if(merger->current_input_file_positions[1] != -1){
      result2 = new_get_next_input_element(merger,1,right);
    }else{
      result2 = EMPTY;
    }
    
    if (result2==FAILURE)
      return FAILURE;
    
    if(result1 == EMPTY && result2 == EMPTY){
      break;
    }
    
    if ((left->UID1==right->UID2) && (left->UID2 == right->UID1) && (left->UID1 < left->UID2)){
      *counter = *counter + 1;
      merger->current_input_buffer_positions[0]++;
      merger->current_input_buffer_positions[1]++;
      merger->output_buffer [merger->current_output_buffer_position].UID1=left->UID1;
      merger->output_buffer [merger->current_output_buffer_position].UID2=left->UID2;
      merger->current_output_buffer_position++;	
      
      if(merger->current_output_buffer_position == merger-> output_buffer_capacity ) {
	if(new_flush_output_buffer(merger)!=SUCCESS) {
	  return FAILURE;			
	  merger->current_output_buffer_position=0;
	}	
      }
    }else if (left->UID1 < right->UID2) {
      merger->current_input_buffer_positions[0]++;
      
    } else if (left->UID1 > right->UID2){
      merger->current_input_buffer_positions[1]++;
    }
    else {
      if (left->UID2 < right->UID1){
	merger->current_input_buffer_positions[0]++;
      } else{
	merger->current_input_buffer_positions[1]++;
      } 
    }    
  }
  
  return SUCCESS;	
}

int join_celebrities(Record *left, Record *right, NewMergeManager * merger, int *counter){
  //stores SUCCESS/FAILURE returned at the end	
  int result1; 
  int result2;
  while (merger->current_input_file_positions[0] != -1 || merger->current_input_file_positions[1] != -1){
    if(merger->current_input_file_positions[0] != -1){
      result1 = new_get_next_input_element(merger,0,left);
    }else{
      result1 = EMPTY;
    }
    
    if (result1==FAILURE)
      return FAILURE;
    
    if(merger->current_input_file_positions[1] != -1){
      result2 = new_get_next_input_element(merger,1,right);
    }else{
      result2 = EMPTY;
    }
    
    if (result2==FAILURE)
      return FAILURE;
    
    if(result1 == EMPTY && result2 == EMPTY){
      break;
    }
    
    if(merger->current_input_file_positions[0] == -1 || left->UID1 > right->UID1){
      merger->current_input_buffer_positions[1]++;
      merger->output_buffer_Celebrities [merger->current_output_buffer_position].UID1=right->UID1;
      merger->output_buffer_Celebrities [merger->current_output_buffer_position].count= right->UID2;
      merger->output_buffer_Celebrities [merger->current_output_buffer_position].indegree = right->UID2;
      merger->output_buffer_Celebrities [merger->current_output_buffer_position].outdegree = 0;
      merger->current_output_buffer_position++;
      if(merger->current_output_buffer_position == merger-> output_buffer_capacity ) {
	if(new_flush_output_buffer(merger)!=SUCCESS) {
	  return FAILURE;			
	  merger->current_output_buffer_position=0;
	}	
      }
    }else if(merger->current_input_file_positions[1] == -1 || left->UID1 < right->UID1){
      merger->current_input_buffer_positions[0]++;
      merger->output_buffer_Celebrities [merger->current_output_buffer_position].UID1=left->UID1;
      merger->output_buffer_Celebrities [merger->current_output_buffer_position].count= -left->UID2;
      merger->output_buffer_Celebrities [merger->current_output_buffer_position].indegree = 0;
      merger->output_buffer_Celebrities [merger->current_output_buffer_position].outdegree = left->UID2;
      merger->current_output_buffer_position++;
      if(merger->current_output_buffer_position == merger-> output_buffer_capacity ) {
	if(new_flush_output_buffer(merger)!=SUCCESS) {
	  return FAILURE;			
	  merger->current_output_buffer_position=0;
	}	
      }
    }else if (left->UID1==right->UID1){
      merger->current_input_buffer_positions[0]++;
      merger->current_input_buffer_positions[1]++;
      merger->output_buffer_Celebrities [merger->current_output_buffer_position].UID1=left->UID1;
      merger->output_buffer_Celebrities [merger->current_output_buffer_position].count=right->UID2 - left->UID2;
      merger->output_buffer_Celebrities [merger->current_output_buffer_position].indegree = right->UID2;
      merger->output_buffer_Celebrities [merger->current_output_buffer_position].outdegree = left->UID2;
      merger->current_output_buffer_position++;
      
      
      if(merger->current_output_buffer_position == merger-> output_buffer_capacity ) {
	if(new_flush_output_buffer(merger)!=SUCCESS) {
	  return FAILURE;			
	  merger->current_output_buffer_position=0;
	}	
      }
    }
    *counter = *counter + 1;
    
    
  }
  
  return SUCCESS;	
}


int new_init_merge (NewMergeManager * manager) {
  FILE *fp_1;
  FILE *fp_2;
  
  if (!(fp_1 = fopen (manager->input_file_name_1, "rb" ))){
    return FAILURE;
    
  }else{
    fseek(fp_1, manager->current_input_file_positions[0]*sizeof(Record), SEEK_SET);
    int result = fread (manager->input_buffers[0], sizeof(Record), manager->input_buffer_capacity, fp_1);
    manager->current_input_file_positions[0] =  result;
    manager->total_input_buffer_elements[0] = result;		
  }	
  fclose(fp_1);
  
  // read for input buffer2 ; 
  if (!(fp_2 = fopen (manager->input_file_name_2, "rb" ))){
    return FAILURE;
    
  }else{
    fseek(fp_2, manager->current_input_file_positions[1]*sizeof(Record), SEEK_SET);
    int result2 = fread (manager->input_buffers[1], sizeof(Record), manager->input_buffer_capacity, fp_2);
    manager->current_input_file_positions[1] =  result2;
    manager->total_input_buffer_elements[1] = result2;		
  }	
  fclose(fp_2);
  
  return SUCCESS;
}

int new_flush_output_buffer (NewMergeManager * manager) {
  FILE *fp_write;
  if (!(fp_write = fopen (manager->output_file_name , "a" ))){
    printf("cannnot true_friends.dat\n");
    return FAILURE;
  }
  //fseek(fp_write, 0L, SEEK_END);
  if(manager->is_query_true_friends == 0){
    fwrite(manager->output_buffer, sizeof(Record), manager->current_output_buffer_position, fp_write);
  }else{
    fwrite(manager->output_buffer_Celebrities, sizeof(CelebritiesRecord), manager->current_output_buffer_position, fp_write);
  }
  
  fflush (fp_write);
  fclose(fp_write);
  manager->current_output_buffer_position = 0;
  return SUCCESS;
}

int new_get_next_input_element(NewMergeManager * manager, int file_number, Record *result) {
  if(manager->current_input_buffer_positions[file_number] == manager->total_input_buffer_elements[file_number]){
    manager->current_input_buffer_positions[file_number] = 0;
    if(new_refill_buffer (manager, file_number)==FAILURE){
      return FAILURE;
    }
    
    if(manager->current_input_file_positions[file_number] == -1){
      return EMPTY;
    }
  }
  *result = manager->input_buffers[file_number][manager->current_input_buffer_positions[file_number]];
  
  return SUCCESS;
}

int new_refill_buffer (NewMergeManager * manager, int file_number) {
  
  
  
  if(file_number == 0){
    FILE *fp_read;
    if (!(fp_read = fopen (manager->input_file_name_1, "rb" ))){
      return FAILURE;
      
    }else{
      fseek(fp_read, manager->current_input_file_positions[0]*sizeof(Record), SEEK_SET);
      int result = fread (manager->input_buffers[0], sizeof(Record), manager->input_buffer_capacity, fp_read);
      if(result <= 0){
	manager->current_input_file_positions[0] = -1;
      }
      manager->current_input_file_positions[0] +=  result;
      manager->total_input_buffer_elements[0] = result;		
    }
    fclose(fp_read);
  }
  
  if(file_number == 1){
    FILE *fp_read;
    if (!(fp_read = fopen (manager->input_file_name_2, "rb" ))){
      return FAILURE;
      
    }else{
      fseek(fp_read, manager->current_input_file_positions[1]*sizeof(Record), SEEK_SET);
      int result = fread (manager->input_buffers[1], sizeof(Record), manager->input_buffer_capacity, fp_read);
      if(result <= 0){
	manager->current_input_file_positions[1] = -1;
      }
      manager->current_input_file_positions[1] +=  result;
      manager->total_input_buffer_elements[1] = result;		
    }
    fclose(fp_read);
  }
  
  return SUCCESS;
}

void new_clean_up (NewMergeManager * merger) {
  int i;
  for(i = 0; i < 2; i++){
    free(merger->input_buffers[i]);
  }
  free(merger->input_buffers);
  if(merger->is_query_true_friends == 0){
    free(merger->output_buffer);
  }else{
    free(merger->output_buffer_Celebrities);
  }
  
  free(merger);
  
}

int compare_heap_elements_new (HeapElement *a, HeapElement *b) {
  if (a->UID2>b->UID2){
    //	if ((a->UID2-b->UID2)>0){
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
