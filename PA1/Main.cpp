#include <stdio.h> 
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include "Ackerman.h"
#include "BuddyAllocator.h"

int main(int argc, char ** argv) {

	int block_size = 128, memory_length = 128 * 1024 * 1024, opt;
	while ((opt = getopt(argc, argv, "b:s:")) != -1) {
  	switch (opt) {
  	case 'b':
    	block_size = atoi(optarg);
    	break;
  	case 's':
    	memory_length = atoi(optarg);
    	break;
  	default:
    	break;
   }
 }
	// create memory manager
	BuddyAllocator * allocator = new BuddyAllocator(block_size, memory_length);

	Ackerman* am = new Ackerman(); // test memory manager
	am->test(allocator); // this is the full-fledged test. 

	delete allocator;  // destroy memory manager
  return 0;
}