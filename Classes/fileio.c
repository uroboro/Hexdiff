#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "fileio.h"

long long sizeOfFile(const char *path) {
	struct stat sr;
	return (stat(path, &sr) != 0)? -1:sr.st_size;
}

unsigned char *bufferFromFile(const char *path, long *size) {
	FILE *fp = fopen(path, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error opening \"%s\"\n", path);
		*size = 0;
		return NULL;
	}

	long long fSize = sizeOfFile(path);
	if (fSize < 0) {
		*size = 0;
		return NULL;
	}


	unsigned char *buffer = (unsigned char *)malloc(fSize * sizeof(unsigned char));
	long bufferSize = 0;

	//load file
	bufferSize = fread(buffer, sizeof(unsigned char), fSize, fp);
	fclose(fp);

	*size = bufferSize;
	return buffer;
}

long saveBufferToFile(char *path, unsigned char *buffer, long length) {
	FILE *fp = fopen(path, "w");
	if (fp == NULL) {
		fprintf(stderr, "Error opening \"%s\"\n", path);
		return 0;
	}

	long size = fwrite(buffer, sizeof(unsigned char), length, fp);
	fclose(fp);
	return size;
}

int filebuffer(FILE *fp, long i) {
	unsigned char c;
	int r = -1;
	if (!feof(fp) && fseek(fp, i, SEEK_SET) == 0 ) {
		if (fread(&c, sizeof(unsigned char), 1, fp) == 1) {
			r = c;
		}
	}
	return r;
}
