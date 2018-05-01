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

    open_and_read(data, "sample2.raw");

    laws_method(K, data);

    open_and_write(K, "K.raw");


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
    int window_size = 37;
    int half = window_size / 2;
    double (*TE)[COL][9] = malloc(sizeof(*TE) * ROW);
    unsigned char T[3][ROW][COL] = {{{0}}};
    int interval_i = 0;
    int interval_j = 0;
    int threshold[3] = {20, 200, 200};
    unsigned char L[ROW][COL];
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
                        total += pow(M[z][x][y], 2);
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
                TE[i][j][z] = (t[z][i][j] - min_value[z]) * 255
                    / (max_value[z] - min_value[z]);
            }
        }
    }

    /* k-means (k=3) */
    memcpy(means[0], TE[100][100], sizeof(*means[0]) * 9);
    memcpy(means[1], TE[385][110], sizeof(*means[1]) * 9);
    memcpy(means[2], TE[385][364], sizeof(*means[2]) * 9);

    do {
        int num[3] = {0};
        double total[3][9] = {{0}};
        flag = 0;
        for (i = 0; i < ROW; i++) {
            for (j = 0; j < COL; j++) {
                int min_group = 0;
                double min_diff = dist(TE[i][j], means[0]);
                for (k = 1; k < 3; k++) {
                    double tmp = dist(TE[i][j], means[k]);
                    if (tmp < min_diff) {
                        min_diff = tmp;
                        min_group = k;
                    }
                }
                num[min_group]++;
                for (z = 0; z < 9; z++) {
                    total[min_group][z] += TE[i][j][z];
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

    /* label with different intensity */
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

    for (i = 0; i < 200; i++)
        for (j = 0; j < 200; j++)
            T[0][i][j] = in[i][j];
    for (i = 0; i < 200; i++)
        for (j = 0; j < 200; j++)
            T[1][i][j] = in[i][j + 310];
    for (i = 0; i < 200; i++)
        for (j = 0; j < 200; j++)
            T[2][i][j] = in[i + 310][j + 310];

    for (z = 0; z < 3; z++) {
        for (i = 10; i < ROW; i++) {
            int k, l;
            long int acc = 0;
            long int diff;
            int count = 0;
            for (k = 0; i + k < 200; k++) {
                for (l = 0; l < 200; l++) {
                    count++;
                    acc += pow(T[z][i + k][l] - T[z][k][l], 2);
                    diff = acc / count;
                    if (diff > threshold[z])
                        goto i_end;
                }
            }
            interval_i = i;
            break;
i_end:;
        }

        for (i = interval_i; i < ROW; i += interval_i) {
            for (k = 0; k < interval_i && i + k < ROW; k++) {
                for (l = 0; l < 200; l++) {
                    T[z][i + k][l] = T[z][k][l];
                }
            }
        }
        /* COL */
        for (j = 10; j < COL; j++) {
            int k, l;
            long int acc = 0;
            long int diff;
            int count = 0;
            for (k = 0; k < 200; k++) {
                for (l = 0; j + l < 200; l++) {
                    count++;
                    acc += pow(T[z][k][j + l] - T[z][k][l], 2);
                    diff = acc / count;
                    if (diff > threshold[z])
                        goto j_end;
                }
            }
            interval_j = j;
            break;
j_end:;
        }

        for (j = interval_j; j < COL; j += interval_j) {
            for (k = 0; k < ROW; k++) {
                for (l = 0; l < interval_j && j + l < COL; l++) {
                    T[z][k][j + l] = T[z][k][l];
                }
            }
        }
        /* test */
        /*
        sprintf(filename, "T%d.raw", z);
        open_and_write(T[z], filename);
        printf("%d: %d\n", z, interval_i);
        printf("%d: %d\n", z, interval_j);
        */
    }

    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            switch (group[i][j]) {
            case 0:
                L[i][j] = T[1][i][j];
                break;
            case 1:
                L[i][j] = T[2][i][j];
                break;
            case 2:
                L[i][j] = T[0][i][j];
                break;
            }
        }
    }
    open_and_write(L, "L.raw");


    free(M);
    free(t);
    free(group);
    free(TE);
}

double dist(double *a, double *b)
{
    int i;
    double total = 0.0;
    int weight[9] = {1, 1, 1, 1, 10, 1, 1, 1, 1};
    for (i = 0; i < 9; i++) {
        total += pow(a[i] - b[i], 2) * weight[i];
    }
    return sqrt(total);
}
