#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

unsigned char *bufferFromFile(char *path, long offset, long length, long *size);

//http://ascii-table.com/ansi-escape-sequences.php
int setColor(int fgc, int bgc, int mode) {
	return fprintf(stdout, "\033[%d;%d;%dm", mode, fgc, 10+bgc);
}

//in case the user interrupts the process (^C)
void __attribute__((destructor)) reserColors(void) {
	setColor(0, 0, 0);
}

int main(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <file1> <file2>\n", argv[0]);
		return 1;
	}

	//TO DO:
	//should add getopts support so it can
	//	make colors optional
	//	set the offset and length of the part to analyse
	//	set lineLength (or make it detect the optimal size?)
	//

	long offset = 0;
	long length = -1;

	char *original = argv[argc - 2];
	char *modified = argv[argc - 1];

	long size1;
	unsigned char *buffer1 = bufferFromFile(original, offset, length, &size1);
	if (buffer1 == NULL || size1 == 0) {
		fprintf(stderr, "Couldn't buffer file: %s.\n", original);
		return 1;
	}

	long size2;
	unsigned char *buffer2 = bufferFromFile(modified, offset, length, &size2);
	if (buffer2 == NULL || size2 == 0) {
		fprintf(stderr, "Couldn't buffer file: %s.\n", modified);
		return 1;
	}

	char lineLength = 16 / 2;
	char colorSupport = 1;

	//print file/s
	long size = (size1 >= size2)? size1:size2;
	long lines = size / lineLength;
	for (long i = 0; i < lines + 1; i++) {
		//print offset
		if (colorSupport == 1) setColor(36, 0, 0);
		fprintf(stdout, "%07x", (int)(i * lineLength));
		if (colorSupport == 1) setColor(0, 0, 0);

		//print line for original file
		for (long j = 0; j < lineLength; j++) {
			long t = i * lineLength + j;
			if (t < size1) {
				//if the other file is smaller or the byte at the same 't' is different, print with a green color
				if (t >= size2 || buffer1[t] != buffer2[t]) {
					if (colorSupport == 1) setColor(32, 0, 0);
					fprintf(stdout, " %02x", buffer1[t]);
					if (colorSupport == 1) setColor(0, 0, 0);
				} else {
					fprintf(stdout, " %02x", buffer1[t]);
				}
			} else {
				fprintf(stdout, "   ");
			}
		}

		//print separator
		if (colorSupport == 1) setColor(36, 0, 0);
		fprintf(stdout, " |");
		if (colorSupport == 1) setColor(0, 0, 0);

		//print line for modified file
		for (long j = 0; j < lineLength; j++) {
			long t = i * lineLength + j;
			if (t < size2) {
				//if the other file is smaller or the byte at the same 't' is different, print with a red color
				if (t >= size1 || buffer1[t] != buffer2[t]) {
					if (colorSupport == 1) setColor(31, 0, 0);
					fprintf(stdout, " %02x", buffer2[t]);
					if (colorSupport == 1) setColor(0, 0, 0);
				} else {
					fprintf(stdout, " %02x", buffer2[t]);
				}
			} else {
				fprintf(stdout, "   ");
			}
		}

		fprintf(stdout, "\n");
	}

	free(buffer1);
	free(buffer2);

	//print final offset
	if (colorSupport == 1) setColor(36, 0, 0);
	fprintf(stdout, "%07x\n", (int)(size));
	if (colorSupport == 1) setColor(0, 0, 0);

	return 0;
}

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
	for (long i = 0; ((length != -1)? i < length:1) && !feof(fp); i++) {
		buffer = (unsigned char *)realloc(buffer, (i + 1) * sizeof(unsigned char));
		bufferSize += fread(&buffer[i], sizeof(unsigned char), 1, fp);
	}
	fclose(fp);

	*size = bufferSize;
	return buffer;
}

long saveBufferToFile(char *path, char *buffer, long length) {
	FILE *fp = fopen(path, "w");
	if (fp == NULL) {
		fprintf(stderr, "Error opening \"%s\"\n", path);
		return 0;
	}

	long size = fwrite(buffer, sizeof(char), length, fp);
	fclose(fp);
	return size;
}
