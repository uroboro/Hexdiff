#include <stdio.h>
#include <stdlib.h>

#include "fileio.h"

unsigned char *bufferFromFile(char *path, long offset, long length, long *size) {
	FILE *fp = fopen(path, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error opening \"%s\"\n", path);
		*size = 0;
		return NULL;
	}

	unsigned char *buffer = (unsigned char *)malloc(sizeof(unsigned char));
	long bufferSize = 0;
	//load file, char by char
	fseek(fp, offset, SEEK_SET);
	for (long i = 0; ((length > 0)? (i < length):1) && !feof(fp); i++) {
		buffer = (unsigned char *)realloc(buffer, (i + 1) * sizeof(unsigned char));
		bufferSize += fread(&buffer[i], sizeof(unsigned char), 1, fp);
	}
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