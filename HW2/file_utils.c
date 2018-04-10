#include "file_utils.h"

#include <stdio.h>
#include <stdlib.h>


void open_and_read(unsigned char (*data)[COL], const char *str)
{
    FILE *fp = fopen(str, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Image files are not in current directory.\n");
        exit(EXIT_FAILURE);
    }
    fread(data, sizeof(unsigned char), ROW * COL, fp);
    fclose(fp);
}

void open_and_write(unsigned char (*data)[COL], const char *str)
{
    FILE *fp = fopen(str, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Can't create file.\n");
        exit(EXIT_FAILURE);
    }
    fwrite(data, sizeof(unsigned char), ROW * COL, fp);
    fclose(fp);
}
