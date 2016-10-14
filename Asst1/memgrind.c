#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>

#include "mymalloc.h"

/*
#define malloc( x ) mymalloc( x, __FILE__, __LINE__ )
#define free( x ) myfree( x, __FILE__, __LINE__ ) */

int main()
{
	mymallocInit();
	printf("Init complete\n");
	long int avg = 0;
	int j = 0;

	/* test 1 */
	avg = 0;
	j = 0;
	void ** temp1[3000];
	int i = 0;
	while(j < 100) {
		printf("%d     ", j);
		struct timeval start, end;
		gettimeofday(&start, NULL);
		i = 0;
		while(i < 3000) {
			temp1[i] = malloc(1);
			i++;
		}
		i = 0;
		while(i < 3000) {
			free(temp1[i]);
			i++;
		}
		long int t = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
		printf("Time : %ld\n", t);
    	avg += t;
		j++;
	}
	avg /= 100;

	printf("Test 1 successful!\nAverage time: %ld sec\n\n", avg);


	return 0;
}