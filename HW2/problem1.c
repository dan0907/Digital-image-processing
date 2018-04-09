#include "file_utils.h"
#include "boundary.h"
#include "histogram.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.1415926535

void first_order(unsigned char (*out)[COL], unsigned char (*in)[COL]);
void second_order(unsigned char (*out)[COL], unsigned char (*in)[COL]);
void canny(unsigned char (*out)[COL], unsigned char (*in)[COL]);
void process(unsigned char (*out)[COL], unsigned char (*in)[COL]);
void gaussian_filter(unsigned char (*out)[COL], unsigned char (*in)[COL]);
void gradient_gen(unsigned char (*magnitude)[COL],
        double (*orientation)[COL], unsigned char (*in)[COL]);
void thresholding(unsigned char (*out)[COL], unsigned char (*in)[COL], 
        unsigned char threshold);
void suppression(unsigned char (*out)[COL],
        unsigned char (*in)[COL], double (*orientation)[COL]);
void h_thresholding(int (*flag)[COL], unsigned char (*in)[COL],
        unsigned char threshold_h, unsigned char threshold_l);
void ccl(unsigned char (*out)[COL], int (*flag)[COL]);




int main(void)
{
    unsigned char s1[ROW][COL];
    unsigned char s2[ROW][COL];
    unsigned char a1[ROW][COL];
    unsigned char a2[ROW][COL];
    unsigned char a3[ROW][COL];
    unsigned char b[ROW][COL];

    open_and_read(s1, "sample1.raw");
    open_and_read(s2, "sample2.raw");

    first_order(a1, s1);
    second_order(a2, s1);
    canny(a3, s1);
    process(b, s2);


    open_and_write(a1, "a1.raw");
    open_and_write(a2, "a2.raw");
    open_and_write(a3, "a3.raw");
    open_and_write(b, "b.raw");

    return 0;
}

void first_order(unsigned char (*out)[COL], unsigned char (*in)[COL])
{
    unsigned char magnitude[ROW][COL];
    double orientation[ROW][COL];
    gradient_gen(magnitude, orientation, in);
    thresholding(out, magnitude, 41);
}
void gradient_gen(unsigned char (*magnitude)[COL],
        double (*orientation)[COL], unsigned char (*in)[COL])
{
    int side_length = 3;
    int half = side_length / 2;
    int total_size = side_length * side_length;
    unsigned char *arr = malloc(sizeof(unsigned char) * total_size);
    int arr_i;
    int i, j, k, l;
    int z = 1;
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            double gr, gc;
            unsigned char tmp;
            for (k = i - half, arr_i = 0; k <= i + half; k++) {
                int x = symmetry(k, ROW);
                for (l = j - half; l <= j + half; l++, arr_i++) {
                    int y = symmetry(l, COL);
                    arr[arr_i] = in[x][y];
                }
            }
            gr = (double)(arr[2] + z * arr[5] + arr[8] - arr[0]
                    - z * arr[3] - arr[6]) / (z + 2);
            gc = (double)(arr[0] + z * arr[1] + arr[2] - arr[6]
                    - z * arr[7] - arr[8]) / (z + 2);
            magnitude[i][j] = sqrt(pow(gr, 2) + pow(gc, 2));
            orientation[i][j] = atan2(gc, gr) * 180 / PI;
            if (orientation[i][j] < 0)
                orientation[i][j] += 180.0;
        }
    }
    free(arr);

}
void thresholding(unsigned char (*out)[COL], unsigned char (*in)[COL], 
        unsigned char threshold)
{
    int i, j;
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            if (in[i][j] >= 41)
                out[i][j] = 255;
            else
                out[i][j] = 0;
        }
    }
}

void suppression(unsigned char (*out)[COL],
        unsigned char (*in)[COL], double (*orientation)[COL])
{
    int i, j;
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            double tmp = orientation[i][j];
            if ((tmp >= 0 && tmp <= 22.5) || (tmp >= 157.5 && tmp <= 180)) {
                if ((j < 0 || in[i][j] > in[i][j - 1])
                        && (j >= COL || in[i][j] > in[i][j + 1])) {
                    out[i][j] = in[i][j];
                } else {
                    out[i][j] = 0;
                }
            } else if (tmp > 22.5 && tmp < 67.5) {
                if ((j < 0 || i >= ROW || in[i][j] > in[i + 1][j - 1])
                        && (j >= COL || i < 0 || in[i][j] > in[i - 1][j + 1])) {
                    out[i][j] = in[i][j];
                } else {
                    out[i][j] = 0;
                }
            } else if (tmp >= 67.5 && tmp <= 112.5) {
                if ((i < 0 || in[i][j] > in[i - 1][j])
                        && (i >= ROW || in[i][j] > in[i + 1][j])) {
                    out[i][j] = in[i][j];
                } else {
                    out[i][j] = 0;
                }
            } else {
                if ((j < 0 || i < 0 || in[i][j] > in[i - 1][j - 1])
                        && (j >= COL || i >= ROW
                            || in[i][j] > in[i + 1][j + 1])) {
                    out[i][j] = in[i][j];
                } else {
                    out[i][j] = 0;
                }
            }
        }
    }
    histogram(out, "suppress");
    cdf(out, "suppress");

}

void second_order(unsigned char (*out)[COL], unsigned char (*in)[COL])
{
    unsigned char gaussian[ROW][COL];
    int laplacian[ROW][COL];
    int side_length = 3;
    int half = side_length / 2;

    double window[3][3] = {
        {-2, 1, -2},
        {1, 4, 1},
        {-2, 1, -2}
    };

    double f = 1.0 / 8.0;
    int i, j, k, l;
    int w_i, w_j;

    int threshold = 2;

    gaussian_filter(gaussian, in);
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            double total = 0;
            for (k = i - half, w_i = 0; k <= i + half; k++, w_i++) {
                int x = symmetry(k, ROW);
                for (l = j - half, w_j = 0; l <= j + half; l++, w_j++) {
                    int y = symmetry(l, COL);
                    total += window[w_i][w_j] * gaussian[x][y];
                }
            }
            laplacian[i][j] = f * total;
        }
    }
    histogram2(laplacian, "laplacian");

    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            if (abs(laplacian[i][j]) <= threshold)
                laplacian[i][j] = 0;
        }
    }


    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            if (laplacian[i][j] == 0) {
                int i_l = symmetry(i - 1, ROW);
                int i_r = symmetry(i + 1, ROW);
                int j_l = symmetry(j - 1, COL);
                int j_r = symmetry(j + 1, COL);
                if (laplacian[i_l][j] * laplacian[i_r][j] < 0
                        || laplacian[i][j_l] * laplacian[i][j_r] < 0
                        || laplacian[i_l][j_l] * laplacian[i_r][j_r] < 0
                        || laplacian[i_l][j_r] * laplacian[i_r][j_l] < 0
                   ) {
                    out[i][j] = 255;
                } else {
                    out[i][j] = 0;
                }
            } else {
                out[i][j] = 0;
            }
        }
    }
}

void canny(unsigned char (*out)[COL], unsigned char (*in)[COL])
{
    unsigned char gaussian[ROW][COL];
    unsigned char magnitude[ROW][COL];
    unsigned char suppressed[ROW][COL];
    double orientation[ROW][COL];
    int flag[ROW][COL] = {0};
    gaussian_filter(gaussian, in);
    gradient_gen(magnitude, orientation, gaussian);
    suppression(suppressed, magnitude, orientation);
    h_thresholding(flag, suppressed, 40, 20);
    ccl(out, flag);
}
void h_thresholding(int (*flag)[COL], unsigned char (*in)[COL],
        unsigned char threshold_h, unsigned char threshold_l)
{
    int i, j;
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            if (in[i][j] >= threshold_h)
                flag[i][j] = 2;
            else if (in[i][j] >= threshold_l)
                flag[i][j] = 1;
        }
    }
}
void ccl(unsigned char (*out)[COL], int (*flag)[COL])
{
    int i, j;
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            int total = 0;
            if (flag[i][j] == 2)
                total = 1;
            else if (flag[i][j] == 1) {
                total = flag[symmetry(i - 1, ROW)][symmetry(j - 1, COL)]
                    + flag[symmetry(i - 1, ROW)][j]
                    + flag[symmetry(i - 1, ROW)][symmetry(j + 1, COL)]
                    + flag[i][symmetry(j - 1, COL)]
                    + flag[i][symmetry(j + 1, COL)]
                    + flag[symmetry(i + 1, ROW)][symmetry(j - 1, COL)]
                    + flag[symmetry(i + 1, ROW)][j]
                    + flag[symmetry(i + 1, ROW)][symmetry(j + 1, COL)];
            }

            if (total)
                out[i][j] = 255;
            else
                out[i][j] = 0;
        }
    }
}

void gaussian_filter(unsigned char (*out)[COL], unsigned char (*in)[COL])
{
    int side_length = 5;
    int half = side_length / 2;

    double window[5][5] = {
        {1, 4, 7, 4, 1},
        {4, 16, 26, 16, 4},
        {7, 26, 41, 26, 7},
        {4, 16, 26, 16, 4},
        {1, 4, 7, 4, 1}
    };

    double f = 1.0 / 273.0;

    int i, j, k, l;
    int w_i, w_j;
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
            out[i][j] = f * total;
        }
    }
}


void process(unsigned char (*out)[COL], unsigned char (*in)[COL])
{
    second_order(out, in);
}
