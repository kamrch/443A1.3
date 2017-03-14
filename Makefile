CC = gcc
CFLAGS = -O3 -Wall 
CFLAGS += -D_LARGEFILE_SOURCE
CFLAGS += -fno-exceptions
CFLAGS += -finline-functions
CFLAGS += -funroll-loops
CFLAGS += -D_FILE_OFFSET_BITS=64
CFLAGS += -std=c99
 
# Source files
DISK_SORT_SRC=disk_sort.c merge_external.c

MERGE_EXTERNAL=merge_external.c

DISTRIBUTION_SRC=distribution.c


 
all: disk_sort merge_external distribution


disk_sort: $(DISK_SORT_SRC) $(DISK_SORT_SRC)
	$(CC) $(CFLAGS) $^ -o disk_sort

merge_external: $(MERGE_EXTERNAL) $(DISK_SORT_SRC)
	$(CC) $(CFLAGS) $^ -o merge_external

distribution: $(DISTRIBUTION_SRC)
	$(CC) $(CFLAGS) $^ -o distribution

clean:  
	rm disk_sort merge_external output*.dat distribution
