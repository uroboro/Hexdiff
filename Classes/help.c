#include <stdio.h>
#include "help.h"

void print_usage(int argc, char **argv) {
	fprintf(stderr, "Usage: %s [options]\n", argv[0]);
	fprintf(stderr, "Shows two columns of hexadecimal values of two different files.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " -f, --file      Set the original file.\n");
	fprintf(stderr, " -m, --modified  Set the modified file.\n");
	fprintf(stderr, " -o, --output    Set the output file.\n");

	fprintf(stderr, " -O, --offset    Set the starting byte offset.\n");
	fprintf(stderr, " -L, --length    Set the length of bytes to compare.\n");

	fprintf(stderr, " -r, --range     Set the starting and ending byte offset range.\n");
	fprintf(stderr, " -R, --Range     Set the starting and ending difference offset range.\n");

	fprintf(stderr, " --linelength    Set the line length.\n");

	fprintf(stderr, " -I, --invert    Inverts selection.\n");
	fprintf(stderr, " -N              Print the number of differences and exit.\n");
	fprintf(stderr, " -D              Write differences to file and exit.\n");
	fprintf(stderr, " --no-color      Disables colors.\n");

	fprintf(stderr, " -h, --help      Display this help\n");
	fprintf(stderr, "\n");
//add examples
/*
	fprintf(stderr, "Examples:\n");
	fprintf(stderr, "%s -r 0:10\n", argv[0]);
	fprintf(stderr, "%s -r 0:10 -r 5:15\n");
	fprintf(stderr, "%s -R 0:10\n");
	fprintf(stderr, "%s -R 0:10 -R 5:15\n");
	fprintf(stderr, "\n");
*/
	fprintf(stderr, "Source code available at https://github.com/uroboro/Hexdiff\n");
}
