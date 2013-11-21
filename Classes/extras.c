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

long getNumberOfDiffs(unsigned char *buffer1, long size1, unsigned char *buffer2, long size2, long diffOffset, long diffLength) {
	if (buffer1 == NULL || size1 <= 0 || buffer2 == NULL || size2 <= 0) {
		return -1;
	}
	if (size1 != size2) {
		fprintf(stderr, "Files differ in size.\n");
		return -1;
	}

	long differences = 0 - diffOffset;
	for (long i = 0; i < size1 && i < size2; i++) {
		if (buffer1[i] != buffer2[i]) {
			differences++;
			if ((diffLength > 0) && (differences == diffLength)) {
				break;
			}
		}
	}
	return differences;
}

long makeFiles(char *filename, unsigned char *buffer1, long size1, unsigned char *buffer2, long size2, long b_count, s_range *b_ranges, long d_count, s_range *d_ranges) {
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

			if (valueIsWithinRanges(b_count, b_ranges, i) && valueIsWithinRanges(d_count, d_ranges, differences)) {
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
