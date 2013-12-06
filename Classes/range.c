#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "range.h"
#include "extras.h"

s_range rangeFromString(char *string) {
	s_range r = {0,0};
	char *a, *b;
	if (splitString(string, &a, &b, RANGE_SEPARATOR) != 0) {
		r.start = atol(a);
		r.stop = atol(b);
		free(a);
		free(b);
	}
	return r;
}

char *stringFromRange(s_range range) {
	char *s;
	asprintf(&s, "{%ld,%ld}", range.start, range.stop);
	return s;
}

void enumRanges(long count, s_range *ranges) {
	fprintf(stdout, "[");
	for (long c = 0; c < count; c++) {
		fprintf(stdout, "{%ld,%ld}", ranges[c].start, ranges[c].stop);
	}
	fprintf(stdout, "]\n");
}

void addRangeToRanges(s_range **ranges, long *count, s_range ra) {
//	fprintf(stderr, "byte range: {%ld:%ld}\n", ra.start, ra.stop);
	if (ra.stop < ra.start) {
		fprintf(stderr, "invalid range.\n");
	} else {
		if (*count == 0 && *ranges == NULL) {
			*ranges = (s_range *)malloc(sizeof(s_range));
		} else {
			*ranges = (s_range *)realloc(*ranges, (*count + 1) * sizeof(s_range));
		}
		(*ranges)[*count].start = ra.start;
		(*ranges)[*count].stop = ra.stop;
		(*count)++;
	}
	return;
}


void sortRanges(long size, s_range *list) {
	if (size <= 0 || list == NULL) {
		return;
	}
	for (long j = 0; j < size - 1; j++) {
		for (long i = 0; i < size - 1 - j; i++) {
			if (list[i].start > list[i+1].start) {
				s_range t = list[i];
				list[i] = list[i+1];
				list[i+1] = t;
			}
		}
	}
	return;
}

//#define print_range(r) printf("{%ld,%ld}", (r).start, (r).stop)
s_range *consolidateRanges(long *size, s_range *list) {
	if (*size <= 0 || list == NULL) {
		return NULL;
	}
	long c_count = 0;
	s_range *c_ranges;

	c_ranges = (s_range *)malloc(sizeof(s_range));
	c_ranges[c_count] = list[0];

	for (long i = 1; i < *size; i++) {
//print_range(c_ranges[c_count]);printf(" vs.");print_range(list[i]);printf("\n");
		if (c_ranges[c_count].stop >= list[i].start) {
			if (c_ranges[c_count].stop < list[i].stop) {
				c_ranges[c_count].stop = list[i].stop;
			}
		} else {
			c_count++;
			c_ranges = (s_range *)realloc(c_ranges, (c_count + 1) * sizeof(s_range));
			c_ranges[c_count] = list[i];
		}
//printf("~");print_range(c_ranges[c_count]);printf(" vs.");print_range(list[i]);printf("\n");
	}

	*size = c_count + 1;
	return c_ranges;
}

char valueIsWithinRanges(long size, s_range *list, long t) {
	char r = 0;
	if (size <= 0 || list == NULL) {
		r = 1;
	} else {
		for (long i = 0; i < size; i++) {
//printf("(%ld:%ld:%ld)", list[i].start, t, list[i].stop);
			if (t >= list[i].start && t <= list[i].stop) {
				r = 1;
				break;
			}
		}
	}
	return r;
}
