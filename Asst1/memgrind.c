#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "mymalloc.h"

/*
#define malloc( x ) mymalloc( x, __FILE__, __LINE__ )
#define free( x ) myfree( x, __FILE__, __LINE__ ) */

int main()
{
	mymallocInit();
	printf("Init complete\n");
	double avg = 0;
	int j = 0;

	/* test 1 */
	avg = 0;
	j = 0;
	void ** temp1[3000];
	int i = 0;
	while(j < 100) {
		printf("%d     ", j);
		clock_t begin = clock();
		i = 0;
		while(i < 3000) {
			temp1[i] = malloc(1);
			i++;
		}
		printf("Done malloc\n");
		i = 0;
		while(i < 3000) {
			free(temp1[i]);
			i++;
		}
		clock_t end = clock();
		avg += (double)(end - begin);
		j++;
	}
	avg /= 100;

	printf("Test 1 successful!\nAverage time: %f sec\n\n", avg);


	return 0;
}
