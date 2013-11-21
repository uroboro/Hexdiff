#ifndef RANGE_H
#define RANGE_H

typedef struct s_range {
    long start;
    long stop;
} s_range;

#define RANGE_SEPARATOR ':'

s_range rangeFromString(char *string);

//bubble sort
void sortRanges(long size, s_range *list);

//remove overlaps
s_range *consolidateRanges(long *size, s_range *list);

//returns 1 if true
char valueIsWithinRanges(long size, s_range *list, long t);

#endif