#include <stdio.h>
#include "help.h"

void print_usage(int argc, char **argv) {
	fprintf(stderr, "Usage: %s [options]\n", argv[0]);
	fprintf(stderr, "Shows two columns of hexadecimal values of two different files.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " -f, --file      Set the original file.\n");
	fprintf(stderr, " -m, --modified  Set the modified file.\n");

	fprintf(stderr, " -o, --offset    Set the starting byte offset.\n");
	fprintf(stderr, " -l, --length    Set the length of bytes to compare.\n");
	fprintf(stderr, " -r, --range     Set the starting and ending byte offset range.\n");

	fprintf(stderr, " -O, --Offset    Set the starting difference offset.\n");
	fprintf(stderr, " -L, --Length    Set the number of differences to compare.\n");
	fprintf(stderr, " -R, --Range     Set the starting and ending difference offset range.\n");

	fprintf(stderr, " --linelength    Set the line length.\n");

	fprintf(stderr, " -N              Print the number of differences and exit.\n");
	fprintf(stderr, " -D              Write differences to file and exit.\n");
	fprintf(stderr, " --no-color      Disables colors.\n");

	fprintf(stderr, " -h, --help      Display this help\n");
	fprintf(stderr, "\n");
//add examples
	fprintf(stderr, "Source code available at https://github.com/uroboro/Hexdiff\n");
}
