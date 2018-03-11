/*
 * CS543 - Operating Systems
 *
 * Program #1 - Data storage
 *
 * This program will show differences in local data storage.
 */
#include <stdio.h>

main()
{
	/*
	 * Let's define a few variables
	 */
	int x;
	int arr[3];
	int y, z;
	int i;
	int *p;

	/*
         * Now let's load some values in them
	 */
	x = 6666;
	for (i=0; i<3; i++ )
		arr[i] = i;
	y = 7777;
	z = 8888;

	/* 
         * Print out current state
         */
        printf("\nOriginal results:\nx = %d, y = %d, z= %d.\n", x, y, z );
	printf("Array = ");
	for (i=0;i<3;i++) printf("%d ", arr[i] );
	printf("\n");

	/* 
         * Can we modify after the array boundary?
         */
	arr[3] = 5;
	arr[4] = 6;
	arr[5] = 7;

	/* 
         * what's there now? 
         */
	printf("\nResults After:\nx = %d, y = %d, z = %d.\n", x, y, z );
	printf("Array = ");
	for (i=0;i<3;i++) printf("%d ", arr[i] );
	printf("\n");

	/*
         * Can I use x to overwrite the array?
         */
	p = &x;
	*p = 4444;  /* should change x */
	for (i=0;i<3;i++) {
		p++;	/* move to next integer */
		*p = 3333;
	}     /* what did I change?  */

	/*
         * Can I use x to overwrite the array?
         */
	p = &x;
	for (i=0;i<2;i++) {
		p--;	/* move to next integer */
		*p = 2222;
	}     /* what did I change?  */

	/* 

	/* 
	 * what's there now?
	 */
	printf("\nResults After Overwrite:\nx = %d, y = %d, z = %d.\n", x, y, z );
	printf("Array = ");
	for (i=0;i<3;i++) printf("%d ", arr[i] );
	printf("\n");

	/*
  	 * can I print out more than what is there?
	 */
	printf("\nPrinting extra elements: \n");
	printf("Array = ");
	for (i=0;i<6;i++) printf("%d ", arr[i] );
	printf("\n");
}
