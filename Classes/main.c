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
	//sizes
	long offset = 0;
	char offsetIsSet = 0;
	long length = 0;
	char lengthIsSet = 0;
	long diffOffset = 0;
	char diffOffsetIsSet = 0;
	long diffLength = 0;
	char diffLengthIsSet = 0;
	char lineLength = 16 / 2;
	char lineLengthIsSet = 0;
	//flags
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
		{"offset",		required_argument,	0, 'o'},
		{"length",		required_argument,	0, 'l'},
		{"range",		required_argument,	0, 'r'},
		{"Offset",		required_argument,	0, 'O'},
		{"Length",		required_argument,	0, 'L'},
		{"Range",		required_argument,	0, 'R'},
		{"linelength",	required_argument,	0, 'W'},
		/* Flag options. */
		{"no-color",	no_argument,		&no_color_flag, 1},
		{"help",		no_argument,		&help_flag, 1},
		{"DEBUG",		no_argument,		&debug_flag, 1},
		/* End of options. */
		{0, 0, 0, 0}
	};
	int opt;
	int option_index = 0;
	while ((opt = getopt_long(argc, argv, "f:m:o:l:r:O:L:R:NDh", long_options, &option_index)) != -1) {
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

		case 'O':
			if (diffOffsetIsSet == 0) {
				diffOffsetIsSet = 1;
				diffOffset = atol(optarg);
			}
			break;

		case 'L':
			if (diffLengthIsSet == 0) {
				diffLengthIsSet = 1;
				diffLength = atol(optarg);
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

/*
printf("found  ranges:[");for(long c=0;c<b_count; c++){printf("{%ld,%ld}", b_ranges[c].start, b_ranges[c].stop);};printf("]\n");
*/
	sortRanges(b_count, b_ranges);
//printf("sorted ranges:[");for(long c=0;c<b_count; c++){printf("{%ld,%ld}", b_ranges[c].start, b_ranges[c].stop);};printf("]\n");
	s_range *t_ranges = consolidateRanges(&b_count, b_ranges);
	free(b_ranges);
	b_ranges = t_ranges;
//printf("cons.  ranges:[");for(long c=0;c<b_count; c++){printf("{%ld,%ld}", b_ranges[c].start, b_ranges[c].stop);};printf("]\n");

	sortRanges(d_count, d_ranges);
	s_range *T_ranges = consolidateRanges(&d_count, d_ranges);
	free(d_ranges);
	d_ranges = T_ranges;


	if (debug_flag == 1) {
		printf("original: %s\n", original);
		printf("modified: %s\n", modified);
		printf("offset: %ld\n", offset);
		printf("length: %ld\n", length);
		printf("diffOffset: %ld\n", diffOffset);
		printf("diffLength: %ld\n", diffLength);
		printf("lineLength: %d\n", lineLength);
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
	char colorSupport = !no_color_flag;
	if (D_flag == 1) {
		offset = 0;
		length = 0;
	}

	//end setup

/*
if (no-buffer) {
	FILE *fp1 = fopen(original, "r");
	if (fp1 == NULL) {
		fprintf(stderr, "Error opening \"%s\"\n", path);
		return 1;
	}
}
*/

	//load original file
	long size1;
	unsigned char *buffer1 = bufferFromFile(original, offset, length, &size1);
	if (buffer1 == NULL || size1 == 0) {
		fprintf(stderr, "Couldn't buffer file: %s.\n", original);
		return 1;
	}

	//load modified file
	long size2;
	unsigned char *buffer2 = bufferFromFile(modified, offset, length, &size2);
	if (buffer2 == NULL || size2 == 0) {
		fprintf(stderr, "Couldn't buffer file: %s.\n", modified);
		return 1;
	}

	long size = (size1 >= size2)? size1:size2;
	long lines = size / lineLength;
	long differences = 0;

	//Print number of differences
	if (N_flag == 1) {
		differences = getNumberOfDiffs(buffer1, size1, buffer2, size2, diffOffset, diffLength);
		fprintf(stdout, "%ld\n", differences);
		return 0;
	}

	//Print differences to files
	if (D_flag == 1) {
		differences = makeFiles(original, buffer1, size1, buffer2, size2, b_count, b_ranges, d_count, d_ranges);
		fprintf(stdout, "Wrote %ld differences.\n", differences);
		return 0;
	}


	//print file/s
	for (long i = 0; i < lines + 1; i++) {
		if (i * lineLength >= size1 || i * lineLength >= size2) {
			break;
		}

		//print offset
		if (colorSupport == 1) setColor(36, 0, 0);
		fprintf(stdout, "%07x", (int)(offset + i * lineLength));
		if (colorSupport == 1) setColor(0, 0, 0);

		//print line for original file
		for (long j = 0; j < lineLength; j++) {
			long t = i * lineLength + j; //absolute offset

			if (t >= size1) {
				fprintf(stdout, " %c ", ' ');
			} else {
				char printWithColor;
				if (t < size2) {
					if (buffer1[t] != buffer2[t]) {
						differences ++;
						printWithColor = valueIsWithinRanges(b_count, b_ranges, t) && valueIsWithinRanges(d_count, d_ranges, differences);
					} else {
						printWithColor = 0;
					}
				} else {
					printWithColor = 1;
				}
				//if the byte at the same 't' is different, print with a green color
				if (printWithColor == 1 && colorSupport == 1) setColor(32, 0, 0);
				fprintf(stdout, " %02x", buffer1[t]);
				if (printWithColor == 1 && colorSupport == 1) setColor(0, 0, 0);
			}
		}

		//print separator
		if (colorSupport == 1) setColor(36, 0, 0);
		fprintf(stdout, " |");
		if (colorSupport == 1) setColor(0, 0, 0);

		//print line for modified file
		for (long j = 0; j < lineLength; j++) {
			long t = i * lineLength + j; //absolute offset

			if (t >= size2) {
				fprintf(stdout, " %c ", ' ');
			} else {
				char printWithColor;
				if (t < size1) {
					if (buffer1[t] != buffer2[t]) {
//						differences ++; //ignore as first part already incremented this
						printWithColor = valueIsWithinRanges(b_count, b_ranges, t) && valueIsWithinRanges(d_count, d_ranges, differences);
					} else {
						printWithColor = 0;
					}
				} else {
					printWithColor = 1;
				}
				//if the byte at the same 't' is different, print with a green color
				if (printWithColor == 1 && colorSupport == 1) setColor(31, 0, 0);
				fprintf(stdout, " %02x", buffer2[t]);
				if (printWithColor == 1 && colorSupport == 1) setColor(0, 0, 0);
			}
		}

/* v2:
			if (t < size2) {
				//if the other file is smaller
				if (t >= size1) {
					if (colorSupport == 1) setColor(31, 0, 0);
					fprintf(stdout, " %02x", buffer2[t]);
					if (colorSupport == 1) setColor(0, 0, 0);
				} else { //if the byte at the same 't' is different, print with a green color
					if (buffer1[t] != buffer2[t]) {
						differences ++;
						if (valueIsWithinRanges(b_count, b_ranges, t) == 1) {
							if (colorSupport == 1) setColor(31, 0, 0);
							fprintf(stdout, " %02x", buffer2[t]);
							if (colorSupport == 1) setColor(0, 0, 0);
						} else { //just print
							fprintf(stdout, " %02x", buffer2[t]);
						}
					} else {
						fprintf(stdout, " %02x", buffer2[t]);
					}
				}
			} else {
				fprintf(stdout, " %s", "  ");
			}
		}
*/
/* v1:
		//print line for modified file
		for (long j = 0; j < lineLength; j++) {
			long t = i * lineLength + j;
			if (t < size2) {
				//if the other file is smaller or the byte at the same 't' is different, print with a red color
				if ((valueIsWithinRanges(b_count, b_ranges, t) == 1) && (t >= size1 || buffer1[t] != buffer2[t])) {
//					differences ++;
					if (colorSupport == 1) setColor(31, 0, 0);
					fprintf(stdout, " %02x", buffer2[t]);
					if (colorSupport == 1) setColor(0, 0, 0);
				} else {
					fprintf(stdout, " %02x", buffer2[t]);
				}
			} else {
				fprintf(stdout, " %s", "  ");
			}
		}
*/
		fprintf(stdout, "\n");
	}

	//print final offset
	if (colorSupport == 1) setColor(36, 0, 0);
	fprintf(stdout, "%07x\n", (int)(offset + size));
	if (colorSupport == 1) setColor(0, 0, 0);

	free(buffer1);
	free(buffer2);

	return r;
}
