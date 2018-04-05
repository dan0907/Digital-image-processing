#include "file_utils.h"

#include <stdio.h>


void open_and_read(unsigned char (*data)[COL], const char *str)
{
    FILE *fp = fopen(str, "rb");
    fread(data, sizeof(unsigned char), ROW * COL, fp);
    fclose(fp);
}

void open_and_write(unsigned char (*data)[COL], const char *str)
{
    FILE *fp = fopen(str, "wb");
    fwrite(data, sizeof(unsigned char), ROW * COL, fp);
    fclose(fp);
}
