#include "defs.h"
#include "file_utils.h"
#include "boundary.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>

void laws_method(unsigned char (*out)[COL], unsigned char (*in)[COL]);
double dist(double *a, double *b);

int main(void)
{
    unsigned char data[ROW][COL];
    unsigned char K[ROW][COL];
    unsigned char L[ROW][COL];

    open_and_read(data, "sample2.raw");

    laws_method(K, data);

    open_and_write(K, "K.raw");

    open_and_write(L, "L.raw");

    return 0;
}

void laws_method(unsigned char (*out)[COL], unsigned char (*in)[COL])
{
    int i, j, k, l, z;
    double (*M)[ROW][COL] = malloc(sizeof(*M) * 9);
    double (*t)[ROW][COL] = malloc(sizeof(*M) * 9);
    int (*group)[COL] = calloc(ROW, sizeof(*group));
    double means[3][9];
    double max_value[9];
    double min_value[9];
    int flag;
    int window_size = 41;
    int half = window_size / 2;
    double (*T)[COL][9] = malloc(sizeof(*T) * ROW);
    double masks[9][3][3] = {
        {
            {1.0/36, 2.0/36, 1.0/36},
            {2.0/36, 4.0/36, 2.0/36},
            {1.0/36, 2.0/36, 1.0/36}
        },
        {
            {1.0/12, 0.0, -1.0/12},
            {2.0/12, 0.0, -2.0/12},
            {1.0/12, 0.0, -1.0/12}
        },
        {
            {-1.0/12, 2.0/12, -1.0/12},
            {-2.0/12, 4.0/12, -2.0/12},
            {-1.0/12, 2.0/12, -1.0/12}
        },
        {
            {-1.0/12, -2.0/12, -1.0/12},
            {0.0, 0.0, 0.0},
            {1.0/12, 2.0/12, 1.0/12}
        },
        {
            {1.0/4, 0.0, -1.0/4},
            {0.0, 0.0, 0.0},
            {-1.0/4, 0.0, 1.0/4}
        },
        {
            {-1.0/4, -2.0/4, -1.0/4},
            {0.0, 0.0, 0.0},
            {1.0/4, -2.0/4, 1.0/4}
        },
        {
            {-1.0/12, -2.0/12, -1.0/12},
            {2.0/12, 4.0/12, 2.0/12},
            {-1.0/12, -2.0/12, -1.0/12}
        },
        {
            {-1.0/4, 0.0, 1.0/4},
            {2.0/4, 0.0, -2.0/4},
            {-1.0/4, 0.0, 1.0/4}
        },
        {
            {1.0/4, -2.0/4, 1.0/4},
            {-2.0/4, 4.0/4, -2.0/4},
            {1.0/4, -2.0/4, 1.0/4},
        }
    };
    for (z = 0; z < 9; z++) {
        for (i = 0; i < ROW; i++) {
            for (j = 0; j < COL; j++) {
                double total = 0.0;
                int w_i, w_j;
                for (k = i - 1, w_i = 0; k <= i + 1; k++, w_i++) {
                    for (l = j - 1, w_j = 0; l <= j + 1; l++, w_j++) {
                        int x = symmetry(k, ROW);
                        int y = symmetry(l, COL);
                        total += masks[z][w_i][w_j] * in[x][y];
                    }
                }
                M[z][i][j] = total;
            }
        }
    }
    for (z = 0; z < 9; z++) {
        max_value[z] = -DBL_MAX;
        min_value[z] = DBL_MAX;
        for (i = 0; i < ROW; i++) {
            for (j = 0; j < COL; j++) {
                double total = 0.0;
                for (k = i - half; k <= i + half; k++) {
                    for (l = j - half; l <= j + half; l++) {
                        int x = symmetry(k, ROW);
                        int y = symmetry(l, COL);
                        total += M[z][x][y] * M[z][x][y];
                    }
                }
                if (total > max_value[z])
                    max_value[z] = total;
                if (total < min_value[z])
                    min_value[z] = total;
                t[z][i][j] = total;
            }
        }
    }

    /* normalize */
    for (z = 0; z < 9; z++) {
        for (i = 0; i < ROW; i++) {
            for (j = 0; j < COL; j++) {
                T[i][j][z] = (t[z][i][j] - min_value[z]) * 255
                    / (max_value[z] - min_value[z]);
            }
        }
    }

    /* k-means (k=3) */
    memcpy(means[0], T[100][100], sizeof(*means) * 9);
    memcpy(means[1], T[385][110], sizeof(*means) * 9);
    memcpy(means[2], T[385][364], sizeof(*means) * 9);

    do {
        int num[3] = {0};
        double total[3][9] = {{0}};
        flag = 0;
        for (i = 0; i < ROW; i++) {
            for (j = 0; j < COL; j++) {
                int min_group = 0;
                double min_diff = dist(T[i][j], means[0]);
                for (k = 1; k < 3; k++) {
                    double tmp = dist(T[i][j], means[k]);
                    if (tmp < min_diff) {
                        min_diff = tmp;
                        min_group = k;
                    }
                }
                num[min_group]++;
                for (z = 0; z < 9; z++) {
                    total[min_group][z] += T[i][j][z];
                }
                if (group[i][j] != min_group) {
                    flag = 1;
                    group[i][j] = min_group;
                }
            }
        }

        for (k = 0; k < 3; k++) {
            for (z = 0; z < 9; z++) {
                means[k][z] = total[k][z] / num[k];
            }
        }

    } while (flag);

    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            switch(group[i][j]) {
            case 0:
                out[i][j] = 0;
                break;
            case 1:
                out[i][j] = 128;
                break;
            case 2:
                out[i][j] = 255;
                break;
            }
        }
    }

    free(M);
    free(t);
    free(group);
    free(T);
}

double dist(double *a, double *b)
{
    int i;
    double total = 0.0;
    int weight[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    weight[4] = 4;
    weight[7] = 4;
    weight[8] = 2;
    for (i = 0; i < 9; i++) {
        total += pow(a[i] - b[i], 2) * weight[i];
    }
    return sqrt(total);
}
