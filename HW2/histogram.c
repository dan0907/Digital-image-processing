#include "histogram.h"
#include <stdio.h>

void histogram(unsigned char (*data)[COL], const char *str)
{
    FILE *fp;
    char buffer[50];
    int max_num = 0;
    int total_points = ROW * COL;
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
        int normalize = arr[i] * 65 / max_num;
        fprintf(fp, "%3d: %3.2f%%, ", i, (double)arr[i] * 100 / total_points);
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
void histogram2(int (*data)[COL], const char *str)
{
    FILE *fp;
    char buffer[50];
    int max_num = 0;
    int total_points = ROW * COL;
    int i, j;
    int arr[511] = {0};
    unsigned char raw[256][511] = {0};
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            int tmp = ++arr[data[i][j] + 255];
            if (tmp > max_num)
                max_num = tmp;
        }
    }

    sprintf(buffer, "histogram2_%s.txt", str);
    fp = fopen(buffer, "w");
    for (i = 0; i < 511; i++) {
        int normalize = arr[i] * 65 / max_num;
        fprintf(fp, "%4d: %3.2f%%, ", i - 255,
                (double)arr[i] * 100 / total_points);
        for (j = 0; j < normalize; j++)
            fputc('*', fp);
        fputc('\n', fp);
    }
    fclose(fp);

    for (i = 0; i < 511; i++) {
        int normalize = arr[i] * 256 / max_num;
        for (j = 0; j < normalize; j++) {
            raw[255 - j][i] = 255;
        }
    }
    sprintf(buffer, "histogram2_%s.raw", str);
    fp = fopen(buffer, "wb");
    fwrite(raw, sizeof(unsigned char), sizeof(raw), fp);
    fclose(fp);
}

void cdf(unsigned char (*data)[COL], const char *str)
{
    FILE *fp;
    char buffer[50];
    int total_points = ROW * COL;
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
        int normalize = arr[i] * 60 / total_points; 
        fprintf(fp, "%3d: %3.2f%%, ", i, (double)arr[i] * 100 / total_points);
        for (j = 0; j < normalize; j++)
            fputc('*', fp);
        fputc('\n', fp);
    }
    fclose(fp);

    for (i = 0; i < 256; i++) {
        int normalize = arr[i] * 256 / total_points;
        for (j = 0; j < normalize; j++) {
            raw[255 - j][i] = 255;
        }
    }
    sprintf(buffer, "cdf_%s.raw", str);
    fp = fopen(buffer, "wb");
    fwrite(raw, sizeof(unsigned char), sizeof(raw), fp);
    fclose(fp);
}
