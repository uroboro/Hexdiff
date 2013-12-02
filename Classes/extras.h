#ifndef EXTRAS_H
#define EXTRAS_H

#include "range.h"

//http://ascii-table.com/ansi-escape-sequences.php
static inline int setColor(int fgc, int bgc, int mode) {
	return fprintf(stdout, "\033[%d;%d;%dm", mode, fgc, 10+bgc);
}

long splitString(char *string, char **a, char **b, char separator);

//pass diffOffset=0 and diffLength=0 for full comparison
long getNumberOfDiffs(unsigned char *buffer1, long size1, unsigned char *buffer2, long size2, long b_count, s_range *b_ranges, long d_count, s_range *d_ranges);

//save differences to file. works best when b_count!=0 with b_ranges a valid list of s_ranges and/or d_count!=0 with d_ranges a valid list of s_ranges
long makeFiles(char *filename, unsigned char *buffer1, long size1, unsigned char *buffer2, long size2, long b_count, s_range *b_ranges, long d_count, s_range *d_ranges);

extern char lineLength, colorSupport, colorPrint, invertSelection;
extern long _offset;

long showDiffs(unsigned char *buffer1, long size1, unsigned char *buffer2, long size2, long b_count, s_range *b_ranges, long d_count, s_range *d_ranges);

#endif
