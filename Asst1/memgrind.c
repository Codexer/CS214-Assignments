#include <stdio.h>

#include "mymalloc.h"

#define malloc( x ) mymalloc( x, __FILE__, __LINE__ )
#define free( x ) myfree( x, __FILE__, __LINE__ ) 

int main()
{
	mymallocInit();
	/* test 1 */

	/* test 2 */
	void * temp;
	int i = 0;
	while(i < 3000)
	{
		temp = malloc(1);
		free(temp);
		i++;
	}
	/* test 3 */

	/* test 4 */

	return 0;
}