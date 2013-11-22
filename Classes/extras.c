#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "extras.h"
#include "fileio.h"

//in case the user interrupts the process (^C)
void __attribute__((destructor)) reserColors(void) {
	setColor(0, 0, 0);
}

void cutFilename(char *path, char **n, char **e) {
	long length = strlen(path);

	long offset = -1;
	for (long i = length - 1; i >= 0; i--) {
		if (path[i] == '.') {
			offset = i;
			break;
		}
	}

	long ns = (offset <= 0)? length:offset;
	char *name = (char *)malloc((ns + 1) * sizeof(char));
	for (long i = 0; i < ns; i++) {
		name[i] = path[i];
	}
	name[ns] = 0;

	long es = (offset <= 0)? 0:length-offset;
	char *extension = (char *)malloc((es + 1) * sizeof(char));
	for (long i = 0; i < es; i++) {
		extension[i] = path[ns+i];
	}
	extension[es] = 0;

	*n = name;
	*e = extension;
	return;
}

long getNumberOfDiffs(unsigned char *buffer1, long size1, unsigned char *buffer2, long size2, long diffOffset, long diffLength, long b_count, s_range *b_ranges, long d_count, s_range *d_ranges) {
	if (buffer1 == NULL || size1 <= 0 || buffer2 == NULL || size2 <= 0) {
		return -1;
	}
	if (size1 != size2) {
		fprintf(stderr, "Files differ in size.\n");
		return -1;
	}

	long differences = 0;
	for (long i = 0; i < size1 && i < size2; i++) {
		if (buffer1[i] != buffer2[i]) {
			differences++;
			if (
 ((differences > diffOffset) == !invertSelection) &&
 ((diffLength > 0)? ((differences < diffOffset + diffLength) == !invertSelection):1) &&
 (valueIsWithinRanges(b_count, b_ranges, i) == !invertSelection) &&
 (valueIsWithinRanges(d_count, d_ranges, differences) == !invertSelection)
) {
				break;
			}
		}
	}
	return differences;
}

long makeFiles(char *filename, unsigned char *buffer1, long size1, unsigned char *buffer2, long size2, long diffOffset, long diffLength, long b_count, s_range *b_ranges, long d_count, s_range *d_ranges) {
	char *name, *extension;
	if (filename == NULL || buffer1 == NULL || size1 <= 0 || buffer2 == NULL || size2 <= 0) {
		return 0;
	}

	cutFilename(filename, &name, &extension);
	char *path;
	asprintf(&path, "%s_diff%s", name, extension);
	free(name);
	free(extension);

	unsigned char *newBuffer = (unsigned char *)malloc(size1 * sizeof(unsigned char));
	if (newBuffer == NULL) {
		fprintf(stderr, "Could not allocate memory for new file.\n");
		return -1;
	}

	long differences = 0;
	long c_dif = 0;

	//process
	for (long i = 0; i < size1 && i < size2; i++) {
		newBuffer[i] = buffer1[i];
		if (buffer1[i] != buffer2[i]) {
			differences ++;

			if (
 ((differences > diffOffset) == !invertSelection) &&
 ((diffLength > 0)? ((differences < diffOffset + diffLength) == !invertSelection):1) &&
 (valueIsWithinRanges(b_count, b_ranges, i) == !invertSelection) &&
 (valueIsWithinRanges(d_count, d_ranges, differences) == !invertSelection)
) {
				c_dif ++;
				//swap different byte
				newBuffer[i] = buffer2[i];
			}
		}
	}
	saveBufferToFile(path, newBuffer, size1);
	free(path);
	free(newBuffer);

	return c_dif;
}

char lineLength, colorSupport, invertSelection;
long _offset;

long showDiffs(unsigned char *buffer1, long size1, unsigned char *buffer2, long size2, long diffOffset, long diffLength, long b_count, s_range *b_ranges, long d_count, s_range *d_ranges) {
	if (buffer1 == NULL || size1 <= 0 || buffer2 == NULL || size2 <= 0) {
		return 0;
	}

	long size = (size1 >= size2)? size1:size2;
	long lines = size / lineLength;
	long differences = 0;
	//print file/s
	for (long i = 0; i < lines + 1; i++) {
		if (i * lineLength >= size1 || i * lineLength >= size2) {
			break;
		}

		//print offset
		if (colorSupport == 1) setColor(36, 0, 0);
		fprintf(stdout, "%07x", (int)(_offset + i * lineLength));
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
						printWithColor = (
 ((differences > diffOffset) == !invertSelection) &&
 ((diffLength > 0)? ((differences < diffOffset + diffLength) == !invertSelection):1) &&
 (valueIsWithinRanges(b_count, b_ranges, t) == !invertSelection) &&
 (valueIsWithinRanges(d_count, d_ranges, differences) == !invertSelection)
);
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
						printWithColor = (
 ((differences >= diffOffset) == !invertSelection) &&
 ((diffLength > 0)? ((differences < diffOffset + diffLength) == !invertSelection):1) &&
 (valueIsWithinRanges(b_count, b_ranges, t) == !invertSelection) &&
 (valueIsWithinRanges(d_count, d_ranges, differences) == !invertSelection)
);					} else {
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

		fprintf(stdout, "\n");
	}

	//print final offset
	if (colorSupport == 1) setColor(36, 0, 0);
	fprintf(stdout, "%07x\n", (int)(_offset + size));
	if (colorSupport == 1) setColor(0, 0, 0);

	return differences;
}