#include "file_utils.h"
#include "boundary.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void edge_crisp(unsigned char (*out)[COL], unsigned char (*in)[COL]);
void warp(unsigned char (*out)[COL], unsigned char (*in)[COL]);
void low_pass_filter(unsigned char (*out)[COL], unsigned char (*in)[COL]);

int main(void)
{
    unsigned char s3[ROW][COL];
    unsigned char C[ROW][COL];
    unsigned char D[ROW][COL];

    open_and_read(s3, "sample3.raw");

    edge_crisp(C, s3);
    warp(D, C);

    open_and_write(C, "C.raw");
    open_and_write(D, "D.raw");

    return 0;
}


void edge_crisp(unsigned char (*out)[COL], unsigned char (*in)[COL])
{
    unsigned char fl[ROW][COL];
    int i, j;
    double c = 0.6;
    low_pass_filter(fl, in);
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            double tmp;
            tmp = c / (2 * c - 1) * in[i][j] - (1 - c) / (2 * c - 1) *
                fl[i][j];
            if (tmp < 0)
                out[i][j] = 0;
            else if (tmp > 255)
                out[i][j] = 255;
            else
                out[i][j] = tmp;
        }
    }
}

void warp(unsigned char (*out)[COL], unsigned char (*in)[COL])
{
}

void low_pass_filter(unsigned char (*out)[COL], unsigned char (*in)[COL])
{
    int side_length = 3;
    int half = side_length / 2;

    double b = 2;

    double denominator = pow(b + 2, 2);
    double tmp1 = 1 / denominator;
    double tmp2 = tmp1 * b;
    double tmp3 = tmp2 * b;
    double window[3][3];

    int i, j, k, l;
    int w_i, w_j;

    for (i = 0; i < side_length; i++) {
        for (j = 0; j < side_length; j++) {
            switch(abs(i - 2) + abs(j - 2)) {
            case 0:
                window[i][j] = tmp3;
                break;
            case 1:
                window[i][j] = tmp2;
                break;
            case 2:
                window[i][j] = tmp1;
            }
        }
    }


    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            double total = 0;
            for (k = i - half, w_i = 0; k <= i + half; k++, w_i++) {
                int x = symmetry(k, ROW);
                for (l = j - half, w_j = 0; l <= j + half; l++, w_j++) {
                    int y = symmetry(l, COL);
                    total += window[w_i][w_j] * in[x][y];
                }
            }
            out[i][j] = total;
        }
    }
}
