/*
File: my_allocator.h

Original Author: R.Bettati
Department of Computer Science
Texas A&M University
Date  : 09/14/18

Modified: by David Qin

*/

#ifndef _BuddyAllocator_h_ // include file only once
#define _BuddyAllocator_h_
#include <iostream>
#include <vector>
using namespace std;
typedef unsigned int uint;

//Holds the individual blocks that will be merged/divided
class BlockHeader {
public:
	uint size;			//Size of memory
	BlockHeader* next;	//Connects to a block that shares the same size
	bool isFree;		//Whether or not the memory is being used
};

//Special linked list that is made out of BlockHeader s. 
class LinkedList {
private:
	BlockHeader* head;		//Head of the list
public:
	void insert(BlockHeader* b) {	//Adds a block to the list
		b->next = head;
		head = b;
	}
	void remove(BlockHeader* b) {	//Removes a block from the list
		BlockHeader* prev;
		BlockHeader* temp;
		prev = NULL;
		temp = head;
		//Finds the location
		while (temp != NULL && temp != b)
		{
			prev = temp;
			temp = temp->next;
		}
		//Called when the list could be empty
		if (prev != NULL)
			prev->next = b->next;
		else
			head = b->next;
	}
	BlockHeader* get_header() {		//Returns header
		return head;
	}
};

//Allocates memory for a program that requests it
class BuddyAllocator {
private:
	vector<LinkedList*> MemoryHeader;	//Holds the various sizes: <minimum szie> * 2^n
	uint block_header_size;				//Size of the header
	uint basic_block_size;				//Size of memory it can allocate
	BlockHeader* Memory;				//The very first head, max size
private:
	/* private function you are required to implement
	this will allow you and us to do unit test */

	char* getbuddy(char * addr);
	// given a block address, this function returns the address of its buddy 

	bool isvalid(char *addr);
	// Is the memory starting at addr is a valid block
	// This is used to verify whether the a computed block address is actually correct 

	bool arebuddies(char* block1, char* block2);
	// checks whether the two blocks are buddies are not

	char* merge(char* block1, char* block2);
	// this function merges the two blocks returns the beginning address of the merged block
	// note that either block1 can be to the left of block2, or the other way around
	// uses recursion
	char* my_merge(char* head);
	//Merge two blocks directly

	BlockHeader* split(BlockHeader* block);
	//Splits the given block by putting a new header halfway through the block recursivley
	BlockHeader* my_split(BlockHeader* head, uint _length);
	//Splits two blocks into smaller addresses

	uint AdjustLengh(uint _length);
	//Adjusts length to nearest power of 2
public:
	BuddyAllocator(uint _basic_block_size, uint _total_memory_length);
	/* This initializes the memory allocator and makes a portion of
	"_total_memory_length" bytes available. The allocator uses a "_basic_block_size" as
	its minimal unit of allocation. The function returns the amount of
	memory made available to the allocator. If an error occurred,
	it returns 0.
	*/

	~BuddyAllocator();
	/* Destructor that returns any allocated memory back to the operating system.
	There should not be any memory leakage (i.e., memory staying allocated).
	*/

	char* alloc(uint _length);
	/* Allocate _length number of bytes of free memory and returns the
	address of the allocated portion. Returns 0 when out of memory. */

	int free(char* _a);
	/* Frees the section of physical memory previously allocated
	using "my_malloc". Returns 0 if everything ok. */

	void debug();
	/* Mainly used for debugging purposes and running short test cases */
	/* This function should print how many free blocks of each size belong to the allocator
	at that point. The output format should be the following (assuming basic block size = 128 bytes):

	128: 5
	256: 0
	512: 3
	1024: 0
	....
	....
	which means that at point, the allocator has 5 128 byte blocks, 3 512 byte blocks and so on.*/
};

#endif 