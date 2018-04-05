#include "histogram.h"
#include <stdio.h>

void histogram(unsigned char (*data)[COL], const char *str)
{
    FILE *fp;
    char buffer[50];
    int max_num = 0;
    int i, j;
    int arr[256] = {0};
    unsigned char raw[256][256] = {0};
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            int tmp = ++arr[data[i][j]];
            if (tmp > max_num)
                max_num = tmp;
        }
    }

    sprintf(buffer, "histogram_%s.txt", str);
    fp = fopen(buffer, "w");
    for (i = 0; i < 256; i++) {
        int normalize = arr[i] * 70 / max_num;
        fprintf(fp, "%3d: ", i);
        for (j = 0; j < normalize; j++)
            fputc('*', fp);
        fputc('\n', fp);
    }
    fclose(fp);

    for (i = 0; i < 256; i++) {
        int normalize = arr[i] * 256 / max_num;
        for (j = 0; j < normalize; j++) {
            raw[255 - j][i] = 255;
        }
    }
    sprintf(buffer, "histogram_%s.raw", str);
    fp = fopen(buffer, "wb");
    fwrite(raw, sizeof(unsigned char), sizeof(raw), fp);
    fclose(fp);
}

void cdf(unsigned char (*data)[COL], const char *str)
{
    FILE *fp;
    char buffer[50];
    int max_num = ROW * COL;
    int i, j;
    int arr[256] = {0};
    unsigned char raw[256][256] = {0};
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            ++arr[data[i][j]];
        }
    }

    for (i = 1; i < 256; i++)
        arr[i] += arr[i - 1];

    sprintf(buffer, "cdf_%s.txt", str);
    fp = fopen(buffer, "w");
    for (i = 0; i < 256; i++) {
        int normalize = arr[i] * 60 / max_num;
        fprintf(fp, "%3d: %3d%%, ", i, arr[i] * 100 / max_num);
        for (j = 0; j < normalize; j++)
            fputc('*', fp);
        fputc('\n', fp);
    }
    fclose(fp);

    for (i = 0; i < 256; i++) {
        int normalize = arr[i] * 256 / max_num;
        for (j = 0; j < normalize; j++) {
            raw[255 - j][i] = 255;
        }
    }
    sprintf(buffer, "cdf_%s.raw", str);
    fp = fopen(buffer, "wb");
    fwrite(raw, sizeof(unsigned char), sizeof(raw), fp);
    fclose(fp);
}
