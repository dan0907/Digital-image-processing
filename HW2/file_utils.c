#include "file_utils.h"

#include <stdio.h>


void open_and_read(uchar (*data)[COL], const char *str)
{
    FILE *fp = fopen(str, "rb");
    fread(data, sizeof(uchar), ROW * COL, fp);
    fclose(fp);
}

void open_and_write(uchar (*data)[COL], const char *str)
{
    FILE *fp = fopen(str, "wb");
    fwrite(data, sizeof(uchar), ROW * COL, fp);
    fclose(fp);
}
