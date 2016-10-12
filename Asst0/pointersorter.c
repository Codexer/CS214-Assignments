#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Convenience methods for warnings/errors
void errorMsg(char *str) {
	printf("ERROR: %s\n", str);
}

void warnMsg(char *str) {
	printf("WARNING: %s\n", str);
}

// Convenience boolean enum
typedef enum {
	TRUE = 1,
	FALSE = 0
} bool;

// Method to gracefully handle weirdo / asshole cases
bool preprocess(int argc, char **argv) {
	if (argc < 2) {
		errorMsg("No argument available.");
		return FALSE;
	}

	if (argc > 2) {
		warnMsg("Wrong number of arguments. Using only first argument.");
	}

	if (strlen(argv[1]) == 0) {
		errorMsg("String needs to have stuff in it.");
		return FALSE;
	}

	return TRUE;
}

// NOT using strcmp
// Using "intuitive" / dictionary order instead
const char *indexToChar = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";

// All hail the ASCII gods for this stupid hash function
int charToIndex(char c) {
	return ((c - 'A') & 31)*2 + ((c - 'A') >> 5);
}

// See http://vigna.di.unimi.it/ftp/papers/Broadword.pdf
// Essentially, it counts groups of bits "in parallel" and uses bit 
// twiddling to add each individual count (also "in parallel")
char popcount(uint64_t bits) {
	bits -= ((bits & 0xAAAAAAAAAAAAAAAA) >> 1);
	bits = (bits & 0x3333333333333333) + ((bits >> 2) & 0x3333333333333333);
	bits = (bits + (bits >> 4)) & 0x0F0F0F0F0F0F0F0F;
	return (bits * 0x0101010101010101) >> 56;
}

uint32_t numWords;
uint32_t maxLength;

uint32_t **indexesByLength;
uint64_t *charTracker;

char **bucketsByLength;

// All the hard work for radix sort
// This method first manually (i.e. no strtok) tokenizes the string
// This method creates:
// -- A list of starting indices for each word, sorted by length
// -- A list of buckets that is used in each length
bool createLists(char *input) {
	uint32_t currPos = 0;
	uint32_t currLen = 0;

	numWords = 0;
	maxLength = 0;

	indexesByLength = NULL;
	charTracker = NULL;

	while (TRUE) {
		// hitting a separator
		if (!isalpha(input[currPos])) {
			// found a new word?
			if (currLen > 0) {
				uint32_t i;
				numWords++;

				// allocate if necessary
				if (currLen > maxLength) {
					indexesByLength = realloc(indexesByLength, currLen * sizeof(*indexesByLength));
					for (i = maxLength; i < currLen; i++) {
						indexesByLength[i] = malloc(sizeof(**indexesByLength));
						indexesByLength[i][0] = 0;
					}

					charTracker = realloc(charTracker, currLen * sizeof(*charTracker));
					for (i = maxLength; i < currLen; i++) {
						charTracker[i] = 0;
					}

					maxLength = currLen;
				} 

				// keep track of characters seen at each position using bit arrays
				for (i = currLen; i > 0; i--) {
					uint64_t bitSet = 1;
					bitSet <<= charToIndex(input[currPos - i]);
					charTracker[currLen - i] |= bitSet;
				}

				// add start index by length (saves current total in first element)
				indexesByLength[currLen - 1][0]++;
				uint32_t numIndexes = indexesByLength[currLen - 1][0];
				indexesByLength[currLen - 1] = realloc(indexesByLength[currLen - 1], (numIndexes + 1) * sizeof(**indexesByLength));
				indexesByLength[currLen - 1][numIndexes] = currPos - currLen;
			}

			// set correct ending
			if (input[currPos] == '\0') {
				break;
			}
			input[currPos] = '\0';

			// reset length
			currLen = 0;
		} else {
			// found a character, add to length
			currLen++;
		}
		currPos++;
	}

	// stupid error checking
	if (numWords == 0) {
		errorMsg("No words found.");
		return FALSE;
	}

	uint32_t currTotal = 0;
	bucketsByLength = malloc(maxLength * sizeof(*bucketsByLength));
	uint32_t lenInd;
	for (lenInd = 0; lenInd < maxLength; lenInd++) {
		int numBuckets = popcount(charTracker[lenInd]);
		bucketsByLength[lenInd] = malloc((numBuckets + 1) * sizeof(**bucketsByLength));
		bucketsByLength[lenInd][numBuckets] = '\0';

		int currInd = 0;
		int charCount = 0;
		while (charTracker[lenInd] != 0) {
			char bitFlag = charTracker[lenInd] & 1;

			bucketsByLength[lenInd][currInd] = bitFlag * indexToChar[charCount];
			currInd += bitFlag;

			charCount++;
			charTracker[lenInd] >>= 1;
		}

		if (numWords - currTotal == numBuckets)	{
			if (lenInd + 1 < maxLength) {
				uint32_t i, j;
				// get all remaining indices and reallocate
				for (i = lenInd + 1; i < maxLength; i++) {
					uint32_t currTotal = indexesByLength[lenInd][0];
					indexesByLength[lenInd] = realloc(indexesByLength[lenInd], (currTotal + indexesByLength[i][0] + 1) * sizeof(**indexesByLength));
					for (j = 0; j < indexesByLength[i][0]; j++) {
						indexesByLength[lenInd][currTotal + j + 1] = indexesByLength[i][j + 1];
					}
					indexesByLength[lenInd][0] += indexesByLength[i][0];
					free(indexesByLength[i]);
				}
				indexesByLength = realloc(indexesByLength, (lenInd + 1) * sizeof(*indexesByLength));
			}
			maxLength = lenInd + 1;
			break;
		}

		currTotal += indexesByLength[lenInd][0];
	}

	return TRUE;
}

// Prints words in sorted order given the start indices of each word
void printInput(char *input, uint32_t* indexList) {
	int i;
	for (i = 0; i < numWords; i++) {
		printf("%s\n", &input[indexList[i]]);
	}
}

uint32_t *radixSort(char *input) {
	// initialize buckets (keeping track of length in first index) and final index list
	uint32_t *indexList = malloc(numWords * sizeof(*indexList));
	uint32_t *buckets[52];
	int i;
	for (i = 0; i < 52; i++) {
		buckets[i] = malloc(sizeof(*buckets[0]));
		buckets[i][0] = 0;
	}

	uint32_t currStart = numWords;
	// unsigned ints wrap around
	uint32_t lenInd;
	for (lenInd = maxLength - 1;; lenInd--) {
		// place all new words in buckets first
		uint32_t i2;
		for (i2 = 0; i2 < indexesByLength[lenInd][0]; i2++) {
			uint32_t startInd = indexesByLength[lenInd][i2 + 1];
			char currChar = input[startInd + lenInd];
			uint32_t *currBucket = buckets[charToIndex(currChar)];
			currBucket[0]++;
			currBucket = realloc(currBucket, (currBucket[0] + 1) * sizeof(*buckets[0]));
			currBucket[currBucket[0]] = startInd;
			buckets[charToIndex(currChar)] = currBucket;
		}

		// place all previously placed into buckets
		uint32_t currInd;
		for (currInd = currStart; currInd < numWords; currInd++) {
			uint32_t startInd = indexList[currInd];
			char currChar = input[startInd + lenInd];
			uint32_t *currBucket = buckets[charToIndex(currChar)];
			currBucket[0]++;
			currBucket = realloc(currBucket, (currBucket[0] + 1) * sizeof(*buckets[0]));
			currBucket[currBucket[0]] = startInd;
			buckets[charToIndex(currChar)] = currBucket;
		}

		// take out of buckets
		currStart -= indexesByLength[lenInd][0];
		uint32_t currTotal = 0;
		int charIndex;
		for (charIndex = 0; charIndex < strlen(bucketsByLength[lenInd]); charIndex++) {
			uint32_t *bucket = buckets[charToIndex(bucketsByLength[lenInd][charIndex])];
			uint32_t i3;
			for (i3 = 0; i3 < bucket[0]; i3++) {
				indexList[currStart + currTotal + i3] = bucket[i3 + 1];
			}
			currTotal += bucket[0];

			free(bucket);
			uint32_t *emptyBucket = malloc(sizeof(*buckets[0]));
			emptyBucket[0] = 0;
			buckets[charToIndex(bucketsByLength[lenInd][charIndex])] = emptyBucket;
		}

		// unsigned ints wrap around
		if (lenInd == 0) {
			break;
		}
	}

	for (i = 0; i < 52; i++) {
		free(buckets[i]);
	}

	return indexList;
}

void cleanUp(uint32_t *indexList) {
	free(charTracker);
	charTracker = NULL;

	free(indexList);
	indexList = NULL;

	uint32_t i;
	for (i = 0; i < maxLength; i++) {
		free(indexesByLength[i]);
		indexesByLength[i] = NULL;
	}
	free(indexesByLength);
	indexesByLength = NULL;

	for (i = 0; i < maxLength; i++) {
		free(bucketsByLength[i]);
		bucketsByLength[i] = NULL;
	}
	free(bucketsByLength);
	bucketsByLength = NULL;
}

int main(int argc, char **argv) {
	if (!preprocess(argc, argv) || !createLists(argv[1])) {
		return 1;
	}
	uint32_t *indexList = radixSort(argv[1]);
	printInput(argv[1], indexList);

	cleanUp(indexList);

	return 0;
}