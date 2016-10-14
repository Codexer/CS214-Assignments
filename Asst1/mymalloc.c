#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>

#include "mymalloc.h"

#define BLOCK_SIZE 15000
#define HEADER_SIZE 2

// #define malloc( x ) mymalloc( x, __FILE__, __LINE__ )
// #define free( x ) myfree( x, __FILE__, __LINE__ )

// Extra 2 to keep track of first free pointer (may not be at beginning)
static char myblock[BLOCK_SIZE + HEADER_SIZE];
static const uint16_t NULL_VALUE = BLOCK_SIZE + 1;

void setIndexToValue(uint16_t index, uint16_t value) {
	*(uint16_t *)(&myblock[index]) = value;
}

uint16_t getValueFromIndex(uint16_t index) {
	return *(uint16_t *)(&myblock[index]);
}

void *getActualPointer(uint16_t pos) {
	return &myblock[pos + HEADER_SIZE];
}

void addBlock(uint16_t pos, uint16_t newSize, block_type isFree) {
	setIndexToValue(pos, (newSize + HEADER_SIZE) + (isFree << 15));
}

uint16_t getLength(uint16_t pos) {
	return (getValueFromIndex(pos) & 0x3fff);
}

uint16_t getSize(uint16_t pos) {
	return getLength(pos) - HEADER_SIZE;
}

bool isType(uint16_t pos, block_type type) {
	return ((getValueFromIndex(pos) >> 15) & 1) == type;
}

uint16_t getNextBlock(uint16_t currPos) {
	return (currPos + getLength(currPos)) % BLOCK_SIZE;
}

uint16_t getNextBlockOfType(uint16_t currPos, block_type type) {
	do {
		currPos = getNextBlock(currPos);
	} while(!isType(currPos, type));

	return currPos;
}

uint16_t getStartAddress() {
	return getValueFromIndex(BLOCK_SIZE);
}

void setStartAddress(uint16_t newPos) {
	setIndexToValue(BLOCK_SIZE, newPos);
}

void printError(char *str, char *fileName, int lineNum) {
	printf("Error in file %s, line %d: %s\n\r", fileName, lineNum, str);
}

void mymallocInit() {
	setStartAddress(0);

	addBlock(0, BLOCK_SIZE - HEADER_SIZE, FREE);
}

void *mymalloc(int size, char *fileName, int lineNum) {
	// get first free block
	uint16_t currPos = getStartAddress();
	uint16_t nextPos = getNextBlockOfType(currPos, FREE);
	uint16_t origPos = currPos;

	if (currPos == NULL_VALUE) {
		printError("No free blocks available.", fileName, lineNum);
		return NULL;
	}

	// while not large enough, keep stepping
	uint16_t currSize;
	while ((currSize = getSize(currPos)) < size) {
		currPos = nextPos;
		nextPos = getNextBlockOfType(currPos, FREE);

		if (currPos == origPos) {
			// if we got through the whole list, fail gracefully
			printError("No free block large enough.", fileName, lineNum);
			return NULL;
		}
	}

	// add used block
	addBlock(currPos, size, USED);

	// otherwise, split (if necessary) and fix free list
	if (currSize == size) {
		// one left: set first index to "NULL"
		if (currPos == nextPos) {
			setStartAddress(NULL_VALUE);
		} else {
			setStartAddress(nextPos);
		}
	} else {
		// need to split, edit old block
		addBlock(currPos + getLength(currPos), currSize - getLength(currPos), FREE);
		setStartAddress(currPos + getLength(currPos));
	}

	return getActualPointer(currPos);
}

void coalesce() {
	uint16_t currPos = 0;
	uint16_t nextPos = getNextBlock(currPos);
	while (nextPos > currPos) {
		if (isType(currPos, FREE) && isType(nextPos, FREE)) {
			addBlock(currPos, getSize(currPos) + getLength(nextPos), FREE);
		} else {
			currPos = nextPos;
		}
		nextPos = getNextBlock(currPos);
	}
}

void myfree(void *ptr, char *fileName, int lineNum) {
	long freePosFull = ((char *)ptr - myblock) - HEADER_SIZE;

	// sanity check #1: is this address in the block?
	if (freePosFull < 0 || freePosFull > BLOCK_SIZE) {
		printError("That address is not in the block.", fileName, lineNum);
		return;
	}

	uint16_t freePos = freePosFull;

	// sanity check #2: is this an allocated address?
	uint16_t currPos = 0;
	uint16_t origPos = currPos;
	while (currPos != freePos) {
		currPos = getNextBlock(currPos);
		if (currPos == origPos) {
			printError("That address was not allocated by mymalloc.", fileName, lineNum);
			return;
		}
	}

	if (isType(currPos, USED)) {
		addBlock(freePos, getSize(freePos), FREE);
		if (freePos < getStartAddress()) {
			setStartAddress(freePos);
		}
		coalesce();
	} else {
		printError("That address was not allocated by mymalloc.", fileName, lineNum);
	}
}

void DEBUG_printBlocksInList(int numBlocks, int startPos) {
	int currPos = startPos;
	for (int i = 0; i < numBlocks; i++) {
		printf("[Pos: %d, Len: %d, Size: %d, Free: %d]", currPos, getLength(currPos), getSize(currPos), isType(currPos, FREE));
		currPos = getNextBlock(currPos);

		if (i != numBlocks - 1) {
			printf(" -> ");
		}
		printf("\n\r");
	}
	printf("-------------\n\r");
}

void DEBUG_printBlockSection(int start, int len) {
	for (int ind = 0; ind < len; ind++) {
		printf("Ind %d: ", ind);
		for (int j = 7; j >= 0; j--) {
			printf("%u", (myblock[start + ind] >> j) & 1);
		}
		printf("\n\r");
	}
}

void DEBUG_printStart() {
	DEBUG_printBlockSection(0, 50);
}

/*int main() {
	mymallocInit();

	int x = 8;
	free(&x);
	free(&myblock[0] - 1);
	free(&myblock[BLOCK_SIZE]);

	malloc(10);
	free(&myblock[5]);

	malloc(20);
	malloc(10);
	malloc(4);
	void *ptr = malloc(1);
	free(ptr);
	DEBUG_printBlocksInList(5, 0);
}*/