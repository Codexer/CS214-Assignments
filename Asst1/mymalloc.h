#ifndef MYMALLOC_H
#define MYMALLOC_H

typedef enum Bool {
	FALSE = 0,
	TRUE = 1
} bool;

typedef enum Type {
	FREE = FALSE,
	USED = TRUE
} block_type;

extern void setIndexToValue(uint16_t index, uint16_t value);
extern uint16_t getValueFromIndex(uint16_t index);
extern void *getActualPointer(uint16_t pos);
extern void addBlock(uint16_t pos, uint16_t newSize, block_type isFree);
extern uint16_t getLength(uint16_t pos);
extern uint16_t getSize(uint16_t pos);
extern bool isType(uint16_t pos, block_type type);
extern uint16_t getNextBlock(uint16_t currPos);
extern uint16_t getNextBlockOfType(uint16_t currPos, block_type type);
extern uint16_t getStartAddress();
extern void setStartAddress(uint16_t newPos);
extern void printError(char *str, char *fileName, int lineNum);
extern void mymallocInit();
extern void *mymalloc(int size, char *fileName, int lineNum);
extern void coalesce();
extern void myfree(void *ptr, char *fileName, int lineNum);
extern void DEBUG_printBlocksInList(int numBlocks, int startPos);
extern void DEBUG_printBlockSection(int start, int len);
extern void DEBUG_printStart();

#endif