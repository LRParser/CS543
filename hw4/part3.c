/*
 * CS543 - Operating Systems
 *
 * Program #3 - Filesystem and executables
 *
 * This program will how different systems deal with executables.
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

main()
{
	/*
	 * Let's see if we can open one of our own executable.
	 */
	int  i, fildes;
	off_t flen;
	char buf[100];
	struct stat statstruct;

	fildes = open("part1", O_WRONLY );
	if ( fildes < 0 ) {
		fprintf(stderr,"can't open part1 for writing.\n");
		return(-1);
	}

	/*
 	 * Now get size of file so that we can blow the file away.
	 */
	if (fstat( fildes, &statstruct ) < 0 ) {
		fprintf(stderr,"can't stat file.\n");
		return(-1);
	}

	/* 
	 * save in var for later 
  	 */
	flen = statstruct.st_size;
	printf("File is %d bytes long.\n", flen );

	/*
 	 * Try to overwrite file with pretty much garbage.
   	 */
	for (i=0; i<flen; i++) {
		buf[0] = i;
		write( fildes, buf, 1 );
	}

	/*
	 * close the file.  now let's try to run it from the shell and
	 * see what happens.
	 */
	 
	close(fildes);
	printf("Try to run part1 from shell.\n");
}
