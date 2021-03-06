/*
 * more01.c - version 0.1 of more
 * read and print 24 lines then pause for a few spacial commands
 *
 * more usage:
 * $ more filename
 * $ command | more
 * $ more < filename
 *
 * +----> show 24 lines form input
 * | +--> print [more?] message
 * | |    Input Enter, SPACE, or q
 * | +--- if Enter, advance one line
 * +----- if SPACE
 *        if q --> exit
 */

#include <stdio.h>
#include <stdlib.h>

#define PAGELEN 24
#define LINELEN 512


void do_more(FILE *fp);
int see_more(void);

int
main(int argc, char *argv[])
{
	FILE *fp;

	if (argc == 1) {
		do_more(stdin);
	} else {
		while (--argc) {
			if ((fp = fopen(*++argv, "r")) != NULL) {
				do_more(fp);
				fclose(fp);
			} else {
				exit(EXIT_FAILURE);
			}
		}
	}

	return 0;
}


/*
 * read PAGELEN lines, then call see_more() for further instructions
 */
void
do_more(FILE *fp)
{
	char line[LINELEN];
	int num_of_lines = 0;
	int reply;

	while (fgets(line, LINELEN, fp)) {
		if (num_of_lines == PAGELEN) {		/* full screen? */
			reply = see_more();		/* y: ask user */
			if (reply == 0)			/* n: done */
				break;
			num_of_lines -= reply;		/* reset count */
		}

		if (fputs(line, stdout) == EOF)		/* show line */
			exit(EXIT_FAILURE);		/* or die */

		num_of_lines++;				/* count it */
	}
}


/*
 * print message, wait for response, return # of lines to advance,
 * q means no, space means yes, CR means one line
 */
int
see_more(void)
{
	int c;
	printf("\033[7m more? \033[m");		/* reverse on a vt100 */
	while ((c = getchar()) != EOF) {	/* get response */
		if (c == 'q')
			return 0;		/* exit */
		else if (c == ' ')
			return PAGELEN;		/* one page */
		else if (c == '\n')
			return 1;		/* one line */
	}
	return 0;
}



