#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>

unsigned char *bufferFromFile(char *path, long offset, long length, long *size);
long saveBufferToFile(char *path, unsigned char *buffer, long length);

//http://ascii-table.com/ansi-escape-sequences.php
static inline int setColor(int fgc, int bgc, int mode) {
	return fprintf(stdout, "\033[%d;%d;%dm", mode, fgc, 10+bgc);
}
//in case the user interrupts the process (^C)
void __attribute__((destructor)) reserColors(void) {
	setColor(0, 0, 0);
}

void cutFilename(char *path, char **n, char **e);

//pass diffOffset=0 and diffLength=0 for full comparison
long getNumberOfDiffs(unsigned char *buffer1, long size1, unsigned char *buffer2, long size2, long diffOffset, long diffLength);

//pass diffOffset=0 and diffLength=0 for all differences
long makeFiles(char *filename, unsigned char *buffer1, long size1, unsigned char *buffer2, long size2, long diffOffset, long diffLength);

void print_usage(int argc, char **argv) {
	fprintf(stderr, "Usage: %s [options]\n", argv[0]);
	fprintf(stderr, "Shows two columns of hexadecimal values of two different files.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " -f, --file	  Set the original file.\n");
	fprintf(stderr, " -m, --modified  Set the modified file.\n");
	fprintf(stderr, " -o, --offset	  Set the starting byte offset.\n");
	fprintf(stderr, " -l, --length	  Set the length of bytes to compare.\n");
	fprintf(stderr, " -O, --Offset	  Set the starting difference offset.\n");
	fprintf(stderr, " -L, --Length	  Set the number of differences to compare.\n");
	fprintf(stderr, " --linelength	  Set the line length.\n");
	fprintf(stderr, " -n			  Show a given number of differences.\n");
	fprintf(stderr, " -N			  Print the number of differences and exit.\n");
	fprintf(stderr, " -D			  Write differences found to files and exit.\n");
	fprintf(stderr, " --no-color	  Disables colors.\n");
	fprintf(stderr, " -h, --help	  Display this help\n");
	fprintf(stderr, "\n");
//add examples
	fprintf(stderr, "Source code available at https://github.com/uroboro/Hexdiff\n");
}

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

	//process options
	struct option long_options[] = {
		/* Flagless options. We distinguish them by their indices. */
		{"file",		required_argument,	0, 'f'},
		{"modified",	required_argument,	0, 'm'},
		{"offset",		required_argument,	0, 'o'},
		{"length",		required_argument,	0, 'l'},
		{"Offset",		required_argument,	0, 'O'},
		{"Length",		required_argument,	0, 'L'},
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
	while ((opt = getopt_long(argc, argv, "f:m:o:l:O:L:ND", long_options, &option_index)) != -1) {
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

/*
		default:
			print_usage(argc, argv);
			exit(EXIT_FAILURE);
*/
		}
	}

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
		differences = makeFiles(original, buffer1, size1, buffer2, size2, diffOffset, diffLength);
		fprintf(stdout, "Wrote %ld files.\n", differences);
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
				fprintf(stdout, " %s", "  ");
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

		fprintf(stdout, "\n");
	}

	free(buffer1);
	free(buffer2);

	//print final offset
	if (colorSupport == 1) setColor(36, 0, 0);
	fprintf(stdout, "%07x\n", (int)(offset + size));
	if (colorSupport == 1) setColor(0, 0, 0);

	return r;
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
	long size = (size1 >= size2)? size1:size2;
	long differences = 0 - diffOffset;
	for (long i = 0; i < size; i++) {
		if (i >= size2 || i >= size1) {
			fprintf(stderr, "Files differ in size.\n");
			break;
		}
		if (buffer1[i] != buffer2[i]) {
			differences++;
			if ((diffLength > 0) && (differences == diffLength)) {
				break;
			}
		}
	}
	return differences;
}

long makeFiles(char *filename, unsigned char *buffer1, long size1, unsigned char *buffer2, long size2, long diffOffset, long diffLength) {
	char *name, *extension;
	cutFilename(filename, &name, &extension);

	long size = (size1 >= size2)? size1:size2;
	long differences = 0 - diffOffset;
	//print file/s
	for (long i = 0; i < size; i++) {
		if (buffer1[i] != buffer2[i]) {
			unsigned char *newBuffer = (unsigned char *)malloc(size1 * sizeof(unsigned char));
			if (newBuffer == NULL) {
				fprintf(stderr, "Could not allocate memory for new file.\n");
				break;
			}
			//copy buffer
			for (long j = 0; j < size1; j++) {
				newBuffer[j] = buffer1[j];
			}
			//swap different byte
			newBuffer[i] = buffer2[i];
			char *path;
			asprintf(&path, "%s_diff-%09x%s", name, i, extension);
			saveBufferToFile(path, newBuffer, size1);
			free(path);
			free(newBuffer);

			differences++;
			if ((diffLength > 0) && (differences == diffLength)) {
				break;
			}
		}
	}

	free(name);
	free(extension);
	return differences;
}
