#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "fileio.h"

long long sizeOfFile(const char *path) {
	struct stat sr;
	return (stat(path, &sr) != 0)? -1:sr.st_size;
}

unsigned char *bufferFromFile(const char *path, long offset, long length, long *size) {
	FILE *fp = fopen(path, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error opening \"%s\"\n", path);
		*size = 0;
		return NULL;
	}

#ifdef CRAPPY && CRAPPY == 1
	long long fSize = 1;
#else
	long long fSize = sizeOfFile(path);
	if (fSize < 0) {
		*size = 0;
		return NULL;
	}
#endif

	unsigned char *buffer = (unsigned char *)malloc(fSize * sizeof(unsigned char));
	long bufferSize = 0;

	//load file, char by char
#ifdef CRAPPY && CRAPPY == 1
	fseek(fp, offset, SEEK_SET);
	for (long i = 0; ((length > 0)? (i < length):1) && !feof(fp); i++) {
		buffer = (unsigned char *)realloc(buffer, (i + 1) * sizeof(unsigned char));
		bufferSize += fread(&buffer[i], sizeof(unsigned char), 1, fp);
	}
#else
	bufferSize = fread(buffer, sizeof(unsigned char), fSize, fp);
#endif
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
