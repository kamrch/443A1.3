CC = gcc
CFLAGS = -O3 -Wall 
CFLAGS += -D_LARGEFILE_SOURCE
CFLAGS += -fno-exceptions
CFLAGS += -finline-functions
CFLAGS += -funroll-loops
CFLAGS += -D_FILE_OFFSET_BITS=64
CFLAGS += -std=c99
 
# Source files

MERGE_EXTERNAL_TRUE_FRIENDS_SRC=merge_external_true_friends.c

TRUE_FRIENDS_SRC=true_friends.c disk_sort.c merge_external.c merge_external_true_friends.c 


 
all: true_friends
	
true_friends: $(TRUE_FRIENDS_SRC)
	$(CC) $(CFLAGS) $^ -o true_friends		

clean:  
	rm true_friends
