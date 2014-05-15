#include <stdio.h>
#include "help.h"

void print_usage(int argc, char **argv) {
	fprintf(stdout, "Usage: %s [options]\n", argv[0]);
	fprintf(stdout, "Shows two columns of hexadecimal values of two different files.\n");
	fprintf(stdout, "\n");
	fprintf(stdout, " -f, --file      Set the original file.\n");
	fprintf(stdout, " -m, --modified  Set the modified file.\n");
	fprintf(stdout, " -o, --output    Set the output file.\n");

	fprintf(stdout, " -O, --offset    Set the starting byte offset.\n");
	fprintf(stdout, " -L, --length    Set the length of bytes to compare.\n");

	fprintf(stdout, " -r, --range     Set the starting and ending byte offset range.\n");
	fprintf(stdout, " -R, --Range     Set the starting and ending difference offset range.\n");

	fprintf(stdout, " --linelength    Set the line length.\n");

	fprintf(stdout, " -I, --invert    Inverts selection.\n");
	fprintf(stdout, " -N              Print the number of differences and exit.\n");
	fprintf(stdout, " -D              Write differences to file and exit.\n");
	fprintf(stdout, " --no-color      Disables colors.\n");

	fprintf(stdout, " -h, --help      Display this help\n");
	fprintf(stdout, "\n");
//add examples
	fprintf(stdout, "Examples:\n");
	fprintf(stdout, "%s -f ./original.mov -m ./mutated.mov -N\n", argv[0]);
	fprintf(stdout, "%s -r 0:10\n", argv[0]);
	fprintf(stdout, "%s -r 0:10 -r 5:15\n", argv[0]);
	fprintf(stdout, "%s -R 0:10\n", argv[0]);
	fprintf(stdout, "%s -R 0:10 -R 5:15\n", argv[0]);
	fprintf(stdout, "\n");

	fprintf(stdout, "Source code available at https://github.com/uroboro/Hexdiff\n");
}
