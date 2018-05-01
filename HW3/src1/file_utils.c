#include "file_utils.h"

#include <stdio.h>
#include <stdlib.h>

#define INPUT_FOLDER "../raw/"
#define OUTPUT_FOLDER "../output_images/"

static char file_name[FILENAME_MAX];

void open_and_read(unsigned char (*data)[COL], const char *str)
{
    FILE *fp;
    sprintf(file_name, INPUT_FOLDER "%s", str);
    fp = fopen(file_name, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Can't open image file.\n");
        exit(EXIT_FAILURE);
    }
    fread(data, sizeof(unsigned char), ROW * COL, fp);
    fclose(fp);
}

void open_and_write(unsigned char (*data)[COL], const char *str)
{
    FILE *fp;
    sprintf(file_name, OUTPUT_FOLDER "%s", str);
    fp = fopen(file_name, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Can't create image file.\n");
        exit(EXIT_FAILURE);
    }
    fwrite(data, sizeof(unsigned char), ROW * COL, fp);
    fclose(fp);
}
