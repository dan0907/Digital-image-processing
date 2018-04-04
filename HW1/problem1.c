#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SIZE 256

void divide_intensity_by_3(unsigned char (*a)[SIZE], unsigned char (*b)[SIZE])
{
    int i, j;
    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++)
            a[i][j] = b[i][j] / 3;
}

void transfer_function(unsigned char (*H)[SIZE], unsigned char (*D)[SIZE])
{
    int i, j;
    int ori_histogram[256] = {0};
    int cdf[256];
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            ori_histogram[D[i][j]]++;
        }
    }
    cdf[0] = ori_histogram[0];
    for (i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + ori_histogram[i];
    }


    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++)
            H[i][j] = (unsigned char)(cdf[D[i][j]] * 255 / (SIZE * SIZE));
}

int symmetry(int n)
{
    if (n < 0)
        return -n + 1;
    if (n >= SIZE)
        return SIZE * 2 - 1 - n;
    return n;
}

void LHE(unsigned char (*L)[256], unsigned char (*D)[256])
{
    int side_length = 31;
    int half = side_length / 2;

    int i, j, k, l;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            unsigned char intensity = D[i][j];
            int histogram[256] = {0};
            int cdf = 0;
            for (k = i - half; k <= i + half; k++) {
                int x = symmetry(k);
                for (l = j - half; l <= j + half; l++) {
                    int y = symmetry(l);
                    histogram[D[x][y]]++;	
                }
            }
            for (k = 0; k <= intensity; k++)
                cdf += histogram[k];
            L[i][j] = (unsigned char)(cdf * 255 / (side_length * side_length));
        }
    }
}

void power_law(unsigned char (*F)[SIZE], unsigned char (*D)[SIZE])
{
    int i, j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            F[i][j] = pow(D[i][j] / 255.0, 1.0 / 3) * 255;
        }
    }
}


int main(void)
{

    FILE *fp;
    unsigned char data[SIZE][SIZE];
    unsigned char D[SIZE][SIZE];
    unsigned char H[SIZE][SIZE];
    unsigned char L[SIZE][SIZE];
    unsigned char F[SIZE][SIZE];

    if ((fp = fopen("sample2.raw", "rb")) == NULL) {
        printf("Error\n");
        exit(1);
    }

    fread(data, sizeof(unsigned char), sizeof(data), fp);
    fclose(fp);

    divide_intensity_by_3(D, data);			

    fp = fopen("D.raw", "wb");
    fwrite(D, sizeof(unsigned char), sizeof(D), fp);
    fclose(fp);

    transfer_function(H, D);

    fp = fopen("H.raw", "wb");
    fwrite(H, sizeof(unsigned char), sizeof(H), fp);
    fclose(fp);

    LHE(L, D);

    fp = fopen("L.raw", "wb");
    fwrite(L, sizeof(unsigned char), sizeof(L), fp);
    fclose(fp);

    power_law(F, D);

    fp = fopen("F.raw", "wb");
    fwrite(F, sizeof(unsigned char), sizeof(F), fp);
    fclose(fp);
    return 0;
}
