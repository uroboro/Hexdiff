#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "extras.h"
#include "fileio.h"

//in case the user interrupts the process (^C)
void __attribute__((destructor)) resetColors(void) {
	if (colorPrint == 1) {
		setColor(0, 0, 0);
	}
}

long splitString(char *string, char **a, char **b, char separator) {
	*a = NULL;
	*b = NULL;

	long length = strlen(string);
	if (string == NULL || length == 0) {
		return 0;
	}

	long long offset = -1;
	for (long i = length - 1; i >= 0; i--) {
		if (string[i] == separator) {
			offset = i;
			break;
		}
	}

	//if never set
	if (offset <= 0) {
		return 0;
	}

	char *p = (char *)malloc((length + 1) * sizeof(char));
	strcpy(p, string);
	//swap separator character for a '\0'. now there are 2 'strings' in a 'string'
	p[offset] = 0;

	*a = (char *)malloc((strlen(p) + 1) * sizeof(char));
	if (*a == NULL) {
		return 0;
	}
	strcpy(*a, p);

	*b = (char *)malloc((strlen(&(p[offset+1])) + 1) * sizeof(char));
	if (*b == NULL) {
		free(*a);
		*a = NULL;
		return 0;
	}
	strcpy(*b, &(p[offset+1]));

	free(p);
	return offset;
}

long getNumberOfDiffs(unsigned char *buffer1, long size1, unsigned char *buffer2, long size2, long b_count, s_range *b_ranges, long d_count, s_range *d_ranges) {
	if (buffer1 == NULL || size1 <= 0 || buffer2 == NULL || size2 <= 0) {
		return -1;
	}
	if (size1 != size2) {
		fprintf(stderr, "Files differ in size.\n");
		return -1;
	}

	long differences = 0;
	long d = 0;
	for (long i = 0; i < size1 && i < size2; i++) {
		if (buffer1[i] != buffer2[i]) {
			differences++;
			if ((valueIsWithinRanges(b_count, b_ranges, i) & valueIsWithinRanges(d_count, d_ranges, differences)) == !invertSelection) {
				d++;
			}
		}
	}
	return d;
}

long makeFiles(char *filepath, unsigned char *buffer1, long size1, unsigned char *buffer2, long size2, long b_count, s_range *b_ranges, long d_count, s_range *d_ranges) {
	if (filepath == NULL || buffer1 == NULL || size1 <= 0 || buffer2 == NULL || size2 <= 0) {
		return 0;
	}

	unsigned char *newBuffer = (unsigned char *)malloc(size1 * sizeof(unsigned char));
	if (newBuffer == NULL) {
		fprintf(stderr, "Could not allocate memory for new file.\n");
		return -1;
	}
	memcpy(newBuffer, buffer1, size1);

	long differences = 0;
	long d = 0;
	for (long i = 0; i < size1 && i < size2; i++) {
		if (buffer1[i] != buffer2[i]) {
			differences++;
			if ((valueIsWithinRanges(b_count, b_ranges, i) & valueIsWithinRanges(d_count, d_ranges, differences)) == !invertSelection) {
				d++;
				//swap different byte
				newBuffer[i] = buffer2[i];
			}
		}
	}
	saveBufferToFile(filepath, newBuffer, size1);
	free(newBuffer);

	return d;
}

char lineLength, colorSupport, colorPrint, invertSelection;
long _offset;

long showDiffs(unsigned char *buffer1, long size1, unsigned char *buffer2, long size2, long b_count, s_range *b_ranges, long d_count, s_range *d_ranges) {
	if (buffer1 == NULL || size1 <= 0 || buffer2 == NULL || size2 <= 0) {
		return 0;
	}

	colorPrint = 1; //enable destructor
	long size = (size1 >= size2)? size1:size2;
	long lines = size / lineLength;
	long differences = 0;
	long d = 0;
	//print file/s
	for (long i = 0; i < lines + 1; i++) {
		if (i * lineLength >= size1 || i * lineLength >= size2) {
			break;
		}

		//print offset
		if (colorSupport == 1) setColor(36, 0, 0);
		fprintf(stdout, "%07x", (int)(_offset + i * lineLength));
		if (colorSupport == 1) setColor(0, 0, 0);

		long tempDiffs = differences;
		//print line for original file
		for (long j = 0; j < lineLength; j++) {
			long t = i * lineLength + j; //absolute offset

			if (t >= size1) {
				fprintf(stdout, " %c ", ' ');
			} else {
				char printWithColor = 0;
				if (t < size2) {
					if (buffer1[t] != buffer2[t]) {
						tempDiffs++;
						if ((valueIsWithinRanges(b_count, b_ranges, t) & valueIsWithinRanges(d_count, d_ranges, tempDiffs)) == !invertSelection) {
							printWithColor = 1;
							d++;
						}
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

		tempDiffs = differences;
		//print line for modified file
		for (long j = 0; j < lineLength; j++) {
			long t = i * lineLength + j; //absolute offset

			if (t >= size2) {
				fprintf(stdout, " %c ", ' ');
			} else {
				char printWithColor = 0;
				if (t < size1) {
					if (buffer1[t] != buffer2[t]) {
						tempDiffs++;
						if ((valueIsWithinRanges(b_count, b_ranges, t) & valueIsWithinRanges(d_count, d_ranges, tempDiffs)) == !invertSelection) {
							printWithColor = 1;
							d++;
						}
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

		differences = tempDiffs;
		fprintf(stdout, "\n");
	}

	//print final offset
	if (colorSupport == 1) setColor(36, 0, 0);
	fprintf(stdout, "%07x\n", (int)(_offset + size));
	if (colorSupport == 1) setColor(0, 0, 0);

	return differences;
}
