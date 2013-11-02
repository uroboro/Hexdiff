#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>

unsigned char *bufferFromFile(char *path, long offset, long length, long *size);

//http://ascii-table.com/ansi-escape-sequences.php
inline int setColor(int fgc, int bgc, int mode) {
	return fprintf(stdout, "\033[%d;%d;%dm", mode, fgc, 10+bgc);
}

//in case the user interrupts the process (^C)
void __attribute__((destructor)) reserColors(void) {
	setColor(0, 0, 0);
}

void print_usage(int argc, char **argv) {
	fprintf(stderr, "Usage: %s [options]\n", argv[0]);
	fprintf(stderr, "Shows two columns of hexadecimal values of two different files.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " -f, --file      Set the original file.\n");
	fprintf(stderr, " -m, --modified  Set the modified file.\n");
	fprintf(stderr, " -o, --offset    Set the starting offset.\n");
	fprintf(stderr, " -l, --lenth     Set the length of bytes to compare.\n");
	fprintf(stderr, " -L              Set the line length.\n");
	fprintf(stderr, " -n              Show a given number of differences.\n");
	fprintf(stderr, " -N              Print the number of differences and exit.\n");
	fprintf(stderr, " --no-color      Disables colors.\n");
	fprintf(stderr, " -h, --help      Display this help\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Source code available at https://github.com/uroboro/Hexdiff\n");
}

int main(int argc, char **argv) {

	//TO DO:
	//should add getopts support so it can
	//	set lineLength (or make it detect the optimal size?)
	//
	long offset = 0;
	long length = -1;
	char lineLength = 16 / 2;
	long nDiffs = -1;
	int no_color_flag = 0;
	int help_flag = 0;
	char N_flag = 0;

	char *original = NULL;
	char *modified = NULL;

	//process options
	struct option long_options[] = {
		/* Flag options. */
		{"no-color",	no_argument,		&no_color_flag, 1},
		{"help",		no_argument,		&help_flag, 1},
		/* Flagless options. We distinguish them by their indices. */
		{"file",		required_argument,	0, 'f'},
		{"modified",	required_argument,	0, 'm'},
		{"offset",		required_argument,	0, 'o'},
		{"length",		required_argument,	0, 'l'},
		{0, 0, 0, 0}
	};
	int opt;
	int option_index = 0;
	while ((opt = getopt_long(argc, argv, "f:m:o:l:L:n:N", long_options, &option_index)) != -1) {
		switch (opt) {
		case 'f':
			original = optarg;
			break;

		case 'm':
			modified = optarg;
			break;

		case 'o':
			offset = atol(optarg);
			break;

		case 'l':
			length = atol(optarg);
			break;

		case 'L':
			lineLength = atol(optarg);
			break;

		case 'n':
			nDiffs = atol(optarg);
			break;

		case 'N':
			N_flag = 1;
			break;

		default:
			print_usage(argc, argv);
			exit(EXIT_FAILURE);
		}
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

	//end setup

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

		for (long i = 0; i < size; i++) {
			if (i >= size2 || i >= size1) {
				fprintf(stderr, "Files differ in size.\n");
				break;
			}
			if (buffer1[i] != buffer2[i]) {
				differences ++;
			}
		}
		fprintf(stdout, "%ld\n", differences);
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
			long t = i * lineLength + j;
			if (t < size1) {
				//if the other file is smaller or the byte at the same 't' is different, print with a green color
				if (t >= size2 || buffer1[t] != buffer2[t]) {
					differences ++;
					if (colorSupport == 1) setColor(32, 0, 0);
					fprintf(stdout, " %02x", buffer1[t]);
					if (colorSupport == 1) setColor(0, 0, 0);
				} else {
					fprintf(stdout, " %02x", buffer1[t]);
				}
			} else {
				fprintf(stdout, "	");
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
					differences ++;
					if (colorSupport == 1) setColor(31, 0, 0);
					fprintf(stdout, " %02x", buffer2[t]);
					if (colorSupport == 1) setColor(0, 0, 0);
				} else {
					fprintf(stdout, " %02x", buffer2[t]);
				}
			} else {
				fprintf(stdout, "	");
			}
		}

		fprintf(stdout, "\n");
	}

	free(buffer1);
	free(buffer2);

	//print final offset
	if (colorSupport == 1) setColor(36, 0, 0);
	fprintf(stdout, "%07x\n", (int)(offset + size));
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
