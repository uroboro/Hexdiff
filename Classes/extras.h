#ifndef EXTRAS_H
#define EXTRAS_H

#include "range.h"

//http://ascii-table.com/ansi-escape-sequences.php
static inline int setColor(int fgc, int bgc, int mode) {
	return fprintf(stdout, "\033[%d;%d;%dm", mode, fgc, 10+bgc);
}

void cutFilename(char *path, char **n, char **e);

//pass diffOffset=0 and diffLength=0 for full comparison
long getNumberOfDiffs(unsigned char *buffer1, long size1, unsigned char *buffer2, long size2, long diffOffset, long diffLength);

//save differences to file. works best when b_count!=0 with b_ranges a valid list of s_ranges and/or d_count!=0 with d_ranges a valid list of s_ranges
long makeFiles(char *filename, unsigned char *buffer1, long size1, unsigned char *buffer2, long size2, long b_count, s_range *b_ranges, long d_count, s_range *d_ranges);

#endif