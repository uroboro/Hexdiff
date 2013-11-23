#ifndef FILEIO_H
#define FILEIO_H

long long sizeOfFile(const char *path);

unsigned char *bufferFromFile(const char *path, long *size);

long saveBufferToFile(char *path, unsigned char *buffer, long length);

int filebuffer(FILE *fp, long i);

#endif
