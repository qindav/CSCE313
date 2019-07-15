/*
File: my_allocator.cpp
*/
#include "BuddyAllocator.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
using namespace std;

BuddyAllocator::BuddyAllocator(uint _basic_block_size, uint _total_memory_length) {
	//Determine block size and memory length
	basic_block_size = pow(2, ceil(log2(_basic_block_size)));
	_total_memory_length = AdjustLengh(_total_memory_length);
	int size = log2(_total_memory_length / basic_block_size);

	//Generate the vector that will hold headers of different block sizes
	for (int i = 0; i<size; i++)
		MemoryHeader.push_back(NULL);

	//Create list and add the initial head of the max block size
	LinkedList* lst = new LinkedList();
	BlockHeader* head = (BlockHeader*)(new char[_total_memory_length]);

	//Creates head
	head->size = _total_memory_length;
	head->next = NULL;
	head->isFree = true;

	//Inserts the head into list
	lst->insert(head);
	MemoryHeader.push_back(lst);
	block_header_size = sizeof(BlockHeader);

	//Used for destructing the allocator
	Memory = head;
}
BuddyAllocator::~BuddyAllocator() {
	delete Memory;	//Delete the max head
}
char* BuddyAllocator::alloc(uint _length) {
	//The caller asks for memory of a certain length
	//calculate size to ther nearest upper power of 2
	unsigned int adjustlength = AdjustLengh(_length + block_header_size);
	LinkedList* lst;
	BlockHeader* head = NULL;
	int index = log2(adjustlength / basic_block_size);

	//Now searches for a block of that size
	for (int i = index; i < MemoryHeader.size(); i++) {
		//Size found, checks if there are blocks
		lst = MemoryHeader[i];
		if (lst != NULL) {
			//Blocks have been found, finds one that is available
			head = lst->get_header();
			if ((head != NULL) && (head->size >= adjustlength)) {
				//Insert the head into the list
				while (head != NULL && head->isFree != true)
					head = head->next;
				if (head != NULL)
					break;
			}
		}
	}

	if (head == NULL)
		return NULL;	//Couldn't find available memory

	else if (head->size > adjustlength * 2)
		head = my_split(head, adjustlength);	//Splits up head if too big
	
	//Memory has now been allocated
	head->isFree = false;
	return (char*)head + block_header_size;
}
uint BuddyAllocator::AdjustLengh(uint _length) {
	//Calculates the upper head size
	//inefficent, takes O(n) time
	int i = 0;
	while (pow(2, i)*basic_block_size < _length)
		i++;
	return pow(2, i)*basic_block_size;
}
int BuddyAllocator::free(char* _a) {
	//Frees block
	BlockHeader* pBlock = (BlockHeader*)(_a - block_header_size);
	pBlock->isFree = true;

	//Try to merge the block back into the list
	my_merge((char*)pBlock);

	return 0;
}
void BuddyAllocator::debug() {
	cout << "Size           Holds     Free" << endl;
	//Will go through the list and print out linked list statuses
	for (int i = 0; i < MemoryHeader.size(); i++)
	{
		int totalblocks = 0;
		int freeblocks = 0;
		LinkedList* lst = MemoryHeader[i];
		BlockHeader* header;

		if (lst != NULL) {	//Block available
			header = lst->get_header();
			//Counts blocks available
			while (header != NULL) {
				totalblocks++;
				if (header->isFree == true)
					freeblocks++;	//Counts free blocks
				header = header->next;
			}
		}
		
		//Print for each block size
		cout << left <<  setw(15) << pow(2, i)*basic_block_size << left
			 << setw(10) << totalblocks << setw(10) << freeblocks << endl;
	}
	cout << endl;
}
char* BuddyAllocator::getbuddy(char * addr) {
	BlockHeader* node = (BlockHeader*)addr;									//Block 1 address
	char * pBuddy = (char*)Memory + ((addr - (char*)Memory) ^ node->size);	//Block 2 address
	
	//Uses arebuddes() to determine if the two are buddied together
	if (arebuddies(addr, pBuddy) == true)
		return pBuddy;
	else
		return NULL;
}
bool  BuddyAllocator::arebuddies(char* block1, char* block2) {
	//Sizes of the two blocks
	int size1 = ((BlockHeader *)block1)->size;
	int size2 = ((BlockHeader *)block2)->size;

	//Checks if sizes are the same 
	//and if the distance is equal to block size
	if (size1 == size2 && abs(block1 - block2) == size1)
		return true;
	else
		return false;
}
char* BuddyAllocator::my_merge(char* head)
{
	//Find the buddy of head
	BlockHeader* pBuddy = (BlockHeader*)getbuddy((char *)head);

	//Checks if buddy is free, merges the two, then calls itself again
	if (pBuddy != NULL && pBuddy->isFree == true) {
		char* p = merge(head, (char*)pBuddy);
		my_merge(p);
	}
	return head;
}
char* BuddyAllocator::merge(char* block1, char* block2)
{
	//Determine which one comes first in the memory
	BlockHeader* pFinal;
	if (block1 > block2)
		pFinal = (BlockHeader*)block2;
	else
		pFinal = (BlockHeader*)block1;

	//Index in memory header list based on size
	int indexOrig = log2(pFinal->size / basic_block_size);

	//Remove it from the list
	LinkedList* lstOrig = MemoryHeader[indexOrig];
	lstOrig->remove((BlockHeader*)block1);
	lstOrig->remove((BlockHeader*)block2);

	//Add the merged block into it's proper location in MemoryHeader
	int indexNew = indexOrig + 1;
	LinkedList* lstNew = MemoryHeader[indexNew];

	//Set the values of the block to accomadate for changes
	pFinal->isFree = true;
	pFinal->size = pFinal->size * 2;
	lstNew->insert(pFinal);
	return (char*)pFinal;
}
BlockHeader* BuddyAllocator::my_split(BlockHeader* block, uint _length)
{
	if (block->size == _length)
		return block;	//Block size is enough, do nothing
	else {
		//Split up the block, return one of them
		BlockHeader* p = split(block);
		return my_split(p, _length);
	}
}
BlockHeader* BuddyAllocator::split(BlockHeader* block)
{
	BlockHeader * tempblock = (BlockHeader *)block;
	uint blocksize = tempblock->size;

	//Remove from original list
	LinkedList* lstOrig;
	int indexOrig = log2(blocksize / basic_block_size);
	lstOrig = MemoryHeader[indexOrig];
	lstOrig->remove(tempblock);

	//Add one into the linked list
	LinkedList* lstNew;
	int indexNew = indexOrig - 1;
	lstNew = MemoryHeader[indexNew];
	if (lstNew == NULL) {
		lstNew = new LinkedList();
		MemoryHeader[indexNew] = lstNew;
	}
	lstNew->insert(tempblock);
	tempblock->size = blocksize / 2;

	//Return the other one for use
	BlockHeader * p = (BlockHeader *)((char*)tempblock + blocksize / 2);
	p->isFree = true;
	p->size = blocksize / 2;
	p->next = NULL;
	lstNew->insert(p);

	return p;
}