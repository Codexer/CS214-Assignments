#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>

#include "mymalloc.h"


#define malloc( x ) mymalloc( x, __FILE__, __LINE__ )
#define free( x ) myfree( x, __FILE__, __LINE__ )

int main()
{
	mymallocInit();
	long int avg = 0;
	int j;
	struct timeval start, end;

	/* test 1 */
	avg = 0;
	void ** temp1[3000];
	int i;
	for(j = 0; j < 100; j++) {
		gettimeofday(&start, NULL);
		for(i = 0; i < 3000; i++)
			temp1[i] = malloc(1);
		for(i = 0; i < 3000; i++)
			free(temp1[i]);
		gettimeofday(&end, NULL);
		long int t = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    	avg += t;
	}
	avg /= 100;

	printf("Test 1 successful!\nAverage time: %ld microseconds\n\n", avg);

	/* test 2 */
	avg = 0;
	for(j = 0; j < 100; j++) {
		gettimeofday(&start, NULL);
		void * temp2;
		for(i = 0; i < 3000; i++) {
			temp2 = malloc(1);
			free(temp2);
		}
		gettimeofday(&end, NULL);
		long int t = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
		//printf("Time : %ld\n", t);
		avg += t;
	}
	avg /= 100;
	printf("Test 2 successful!\nAverage time: %ld microseconds\n\n", avg);

	/* test 3 */
	avg = 0;
	srand(time(NULL));
	for(j = 0; j < 100; j++) {
		gettimeofday(&start, NULL);
		int m = 0;
		int f = 0;
		for(i = 0; i < 6000; i++) {
			if(m != 3000 && f != m) {
				int a = rand() % 2;
				if(a == 0) {
					temp1[m - f] = malloc(1);
					m++;
				}
				else {
					f++;
					free(temp1[m - f]);
				}
			}
			else if(m == 3000) {
				f++;
				free(temp1[m - f]);
			}
			else {
				temp1[m - f] = malloc(1);
				m++;
			}
		}
		gettimeofday(&end, NULL);
		long int t = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
		//printf("Time : %ld\n", t);
		avg += t;
	}
	avg /= 100;
	printf("Test 3 successful!\nAverage time: %ld microseconds\n\n", avg);

	/* test 4 */
	avg = 0;
	for(j = 0; j < 100; j++) {
		gettimeofday(&start, NULL);
		int m = 0;
		int f = 0;
		int max = 14996;
		int sizes[3000];
		for(i = 0; i < 6000; i++) {
			if(m != 3000 && f != m && max > 3) {
				int a = rand() % 2;
				if(a == 0) {
					a = rand() % (max - 2);
					temp1[m - f] = malloc(a);
					sizes[m - f] = a;
					max -= 2 + a;
					m++;
				}
				else {
					f++;
					free(temp1[m - f]);
					max += 2 + sizes[m - f];
					sizes[m - f] = 0;
				}
			}
			else if(m == 3000 || max <= 3) {
				f++;
				free(temp1[m - f]);
				max += 2 + sizes[m - f];
				sizes[m - f] = 0;
			}
			else {
				int a = rand() % (max - 2);
				temp1[m - f] = malloc(a);
				sizes[m - f] = a;
				max -= 2 + a;
				m++;
			}
		}
		gettimeofday(&end, NULL);
		long int t = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
		//printf("Time : %ld\n", t);
		avg += t;
	}
	avg /= 100;
	printf("Test 4 successful!\nAverage time: %ld microseconds\n\n", avg);

	/* test 5 */
	avg = 0;
	for(j = 0; j < 100; j++) {
		gettimeofday(&start, NULL);

		//stuff
		int m = 0;
		int f = 0;
		int max = 5000;
		int sizes[3000];
		while(m < 3000) {
			while(m < 3000 && max > 5) {
				int a = rand() % (max - 4);
				temp1[m - f] = malloc(a);
				sizes[m - f] = a;
				max -= a;
				max -= 2; 
				m++;
			}
			while(m < 3000 && f != m)
			{
				f++;
				free(temp1[m - f]);
				max += 2 + sizes[m - f];
				sizes[m - f] = 0;
			}
		}
		gettimeofday(&end, NULL);
		long int t = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
		//printf("Time : %ld\n", t);
		avg += t;
	}
	avg /= 100;
	printf("Test 5 successful!\nAverage time: %ld microseconds\n\n", avg);

	/* test 6 */
/*	avg = 0;
	for(j = 0; j < 100; j++) {
		gettimeofday(&start, NULL);
		int max = 5000;
		for(i = 0; i < 1500; i++)
		{
			int a = rand() % (max - 2);
			void * p1 = malloc(a);
			int b = rand() % (max - 2);
			void * p2 = malloc(b);
			if(p1 != NULL)
				free(p1);
			if(p2 != NULL)
				free(p2);
		}
		
		gettimeofday(&end, NULL);
		long int t = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
		printf("Time : %ld\n", t);
		avg += t;
	}
	avg /= 100;
	printf("Test 6 successful!\nAverage time: %ld microseconds\n\n", avg);
*/
	return 0;
}