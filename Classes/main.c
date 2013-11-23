#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
//#include <ctype.h>
#include <getopt.h>

#include "fileio.h"
#include "range.h"
#include "extras.h"
#include "help.h"

extern char lineLength, colorSupport, invertSelection;
extern long _offset;

int main(int argc, char **argv) {
	int r = 0;

	//TO DO:
	//should add getopts support so it can
	//	auto detect the optimal size for lineLength?
	//
	//filenames
	char *original = NULL;
	char originalIsSet = 0;
	char *modified = NULL;
	char modifiedIsSet = 0;
	char *output = NULL;
	char outputIsSet = 0;
	//sizes
/*
	long offset = 0;
	char offsetIsSet = 0;
	long length = 0;
	char lengthIsSet = 0;
*/
	lineLength = 16 / 2;
	char lineLengthIsSet = 0;
	//flags
	int invert_flag = 0;
	int no_color_flag = 0;
	int help_flag = 0;
	int debug_flag = 0;
	char N_flag = 0;
	char D_flag = 0;

	s_range *b_ranges = NULL;
	long b_count = 0;
	s_range *d_ranges = NULL;
	long d_count = 0;

	//process options
	struct option long_options[] = {
		/* Flagless options. We distinguish them by their indices. */
		{"file",		required_argument,	0, 'f'},
		{"modified",	required_argument,	0, 'm'},
		{"output",		required_argument,	0, 'o'},
		{"offset",		required_argument,	0, 'O'},
		{"length",		required_argument,	0, 'L'},
		{"range",		required_argument,	0, 'r'},
		{"Range",		required_argument,	0, 'R'},
		{"linelength",	required_argument,	0, 'W'},
		/* Flag options. */
		{"invert",		no_argument,		&invert_flag, 1},
		{"no-color",	no_argument,		&no_color_flag, 1},
		{"help",		no_argument,		&help_flag, 1},
		{"DEBUG",		no_argument,		&debug_flag, 1},
		/* End of options. */
		{0, 0, 0, 0}
	};
	int opt;
	int option_index = 0;
	while ((opt = getopt_long(argc, argv, "f:m:o:r:R:INDh", long_options, &option_index)) != -1) {
		switch (opt) {
		case 'f':
			if (originalIsSet == 0) {
				originalIsSet = 1;
				original = optarg;
			}
			break;

		case 'm':
			if (modifiedIsSet == 0) {
				modifiedIsSet = 1;
				modified = optarg;
			}
			break;

		case 'o':
			if (outputIsSet == 0) {
				outputIsSet = 1;
				output = optarg;
			}
			break;

/*
		case 'o':
			if (offsetIsSet == 0) {
				offsetIsSet = 1;
				offset = atol(optarg);
			}
			break;

		case 'l':
			if (lengthIsSet == 0) {
				lengthIsSet = 1;
				length = atol(optarg);
			}
			break;
*/
		case 'r':
			{
				s_range ra = rangeFromString(optarg);
//				printf("byte range: %ld:%ld\n", ra.start, ra.stop);
				if (ra.stop < ra.start) {
					fprintf(stderr, "invalid range.\n");
				} else {
					if (b_count == 0 && b_ranges == NULL) {
						b_ranges = (s_range *)malloc(sizeof(s_range));
					} else {
						b_ranges = (s_range *)realloc(b_ranges, (b_count + 1) * sizeof(s_range));
					}
					b_ranges[b_count].start = ra.start;
					b_ranges[b_count].stop = ra.stop;
					b_count++;
				}
			}
			break;

		case 'R':
			{
				s_range ra = rangeFromString(optarg);
//				printf("byte range: %ld:%ld\n", ra.start, ra.stop);
				if (ra.stop < ra.start) {
					fprintf(stderr, "invalid range.\n");
				} else {
					if (d_count == 0 && d_ranges == NULL) {
						d_ranges = (s_range *)malloc(sizeof(s_range));
					} else {
						d_ranges = (s_range *)realloc(d_ranges, (d_count + 1) * sizeof(s_range));
					}
					d_ranges[d_count].start = ra.start;
					d_ranges[d_count].stop = ra.stop;
					d_count++;
				}
			}
			break;

		case 'W':
			if (lineLengthIsSet == 0) {
				lineLengthIsSet = 1;
				lineLength = atol(optarg);
			}
			break;

		case 'I':
			invert_flag = 1;
			break;

		case 'N':
			N_flag = 1;
			break;

		case 'D':
			D_flag = 1;
			break;

		case 'h':
			help_flag = 1;
			break;

/*
		default:
			print_usage(argc, argv);
			exit(EXIT_FAILURE);
*/
		}
	}

	//process byte ranges
	sortRanges(b_count, b_ranges);
	s_range *t_ranges = consolidateRanges(&b_count, b_ranges);
	free(b_ranges);
	b_ranges = t_ranges;

	//process diff ranges
	sortRanges(d_count, d_ranges);
	s_range *T_ranges = consolidateRanges(&d_count, d_ranges);
	free(d_ranges);
	d_ranges = T_ranges;


	if (debug_flag == 1) {
		printf("original: %s\n", original);
		printf("modified: %s\n", modified);
		printf("output: %s\n", output);
//		printf("offset: %ld\n", offset);
//		printf("length: %ld\n", length);
		printf("lineLength: %d\n", lineLength);
		printf("invert_flag: %d\n", invert_flag);
		printf("no_color_flag: %d\n", no_color_flag);
		printf("help_flag: %d\n", help_flag);
		printf("N_flag: %d\n", N_flag);
		printf("D_flag: %d\n", D_flag);
		printf("b_ranges: [");for(long c=0;c<b_count; c++){printf("{%ld,%ld}", b_ranges[c].start, b_ranges[c].stop);}printf("]\n");
		printf("d_ranges: [");for(long c=0;c<d_count; c++){printf("{%ld,%ld}", d_ranges[c].start, d_ranges[c].stop);}printf("]\n");
	}

	if (help_flag) {
		print_usage(argc, argv);
		return 1;
	}

	if (!original || !modified) {
		fprintf(stderr, "No files to compare.\n");
		return 1;
	}
	if (strcmp(original, modified) == 0) {
		fprintf(stderr, "Same file.\n");
		return 1;
	}

	if (!output) {
		char *name, *extension;
		cutFilename(original, &name, &extension);
		asprintf(&output, "%s_diff%s", name, extension);
		free(name);
		free(extension);
	}

	colorSupport = !no_color_flag;
	invertSelection = invert_flag;
	//end setup


	//load original file
	long size1;
	unsigned char *buffer1 = bufferFromFile(original, &size1);
	if (buffer1 == NULL || size1 == 0) {
		fprintf(stderr, "Couldn't buffer file: %s.\n", original);
		return 1;
	}

	//load modified file
	long size2;
	unsigned char *buffer2 = bufferFromFile(modified, &size2);
	if (buffer2 == NULL || size2 == 0) {
		fprintf(stderr, "Couldn't buffer file: %s.\n", modified);
		return 1;
	}

	long differences = 0;

	//Print number of differences
	if (N_flag == 1) {
		differences = getNumberOfDiffs(buffer1, size1, buffer2, size2, b_count, b_ranges, d_count, d_ranges);
		fprintf(stdout, "%ld\n", differences);
		return 0;
	}

	//Print differences to files
	if (D_flag == 1) {
		differences = makeFiles(output, buffer1, size1, buffer2, size2, b_count, b_ranges, d_count, d_ranges);
		fprintf(stdout, "Wrote %ld differences.\n", differences);
		return 0;
	}

	//Print differences view
_offset = 0;
//_offset = offset;
	differences = showDiffs(buffer1, size1, buffer2, size2, b_count, b_ranges, d_count, d_ranges);

	free(buffer1);
	free(buffer2);

	return r;
}
