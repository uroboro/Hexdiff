#ifndef FILEIO_H
#define FILEIO_H

unsigned char *bufferFromFile(char *path, long offset, long length, long *size);

long saveBufferToFile(char *path, unsigned char *buffer, long length);

int filebuffer(FILE *fp, long i);

#endif
