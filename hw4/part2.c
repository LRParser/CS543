/*
 * CS543 - Operating Systems
 *
 * Program #2 - Data Linkage
 *
 * This program will show differences in data linkage between procedures.
 */
#include <stdio.h>
#include <time.h>

extern char *strptime();

main()
{
	/*
    	 * strptime has a signature of 
	 *   char *strptime( const char *s, const char *format, 
  	 *              struct tm *tm);
 	 */
	char string1[10], string2[10], *p;
	struct tm mytime;
	int i;

	printf("Trying to call strptime with not enough parameters.\n");
	p = strptime( string1, string2 );     	
	if ( p == NULL ) 
		printf("returned NULL.\n");
	else
		printf("returned something.\n");

	printf("Trying to call strptime with extra parameters.\n");
	p = strptime( string1, string2, &mytime, i );

	if ( p == NULL ) 
		printf("returned NULL.\n");
	else
		printf("returned something.\n");

	printf("Exiting program successfully.\n");
	return(0);
}
