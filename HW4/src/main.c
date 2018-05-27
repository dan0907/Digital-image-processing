#include "file_utils.h"
#include "disjoint_set.h"
#include "boundary.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void median_filter(unsigned char (*out)[COL], unsigned char (*in)[COL]);
void threshold_filter(unsigned char (*out)[COL], unsigned char (*in)[COL]);
void threshold_filter_tr(unsigned char (*out)[COL_TR],
        unsigned char (*in)[COL_TR]);
int cmp(const void *a, const void *b);
void process_tt(unsigned char (*in1)[COL], unsigned char (*in2)[COL]);
void process_tr(unsigned char (*in)[COL_TR]);

struct feature {
    int width;
    int height;
    int holes;
    int ccs;
};

struct test_s {
    char real;
    int w_start;
    int w_end;
    int h_start;
    int h_end;
    unsigned char *im;
    struct feature f;
};

struct train_s {
    char real;
    char predict;
    int w_start;
    int w_end;
    int h_start;
    int h_end;
    unsigned char *im;
    struct feature f;
};

struct slice_s {
    int h_start;
    int h_end;
};

struct test_s test_c[11];
struct train_s train_c[70];
struct slice_s slices[5];

int main(void)
{
    unsigned char s1[ROW][COL];
    unsigned char s2[ROW][COL];
    unsigned char s1_pre1[ROW][COL];
    unsigned char s2_pre1[ROW][COL];
    unsigned char s2_pre2[ROW][COL];
    unsigned char s2_pre3[ROW][COL];
    unsigned char train[ROW_TR][COL_TR];
    unsigned char train_pre[ROW_TR][COL_TR];
    open_and_read(s1, "sample1.raw");
    open_and_read(s2, "sample2.raw");
    open_and_read_tr(train, "TrainingSet.raw");
    median_filter(s2_pre1, s2);
    open_and_write(s2_pre1, "s2_pre1.raw");
    median_filter(s2_pre2, s2_pre1);
    open_and_write(s2_pre2, "s2_pre2.raw");
    threshold_filter(s2_pre3, s2_pre2);
    open_and_write(s2_pre3, "s2_pre3.raw");

    threshold_filter(s1_pre1, s1);
    open_and_write(s1_pre1, "s1_pre1.raw");
    threshold_filter_tr(train_pre, train);
    open_and_write_tr(train_pre, "train_pre.raw");

    process_tt(s1_pre1, s2_pre3);
    process_tr(train_pre);
    open_and_write(s1_pre1, "s1_box.raw");
    open_and_write(s2_pre3, "s2_box.raw");
    open_and_write_tr(train_pre, "train_box.raw");

    return 0;
}

int cmp(const void *a, const void *b)
{
    return *(unsigned char *)a - *(unsigned char *)b;
}

void median_filter(unsigned char (*out)[COL], unsigned char (*in)[COL])
{
    int side_length = 3;
    int half = side_length / 2;
    int total_size = side_length * side_length;
    unsigned char *arr = malloc(sizeof(*arr) * total_size);
    int arr_i;
    int median = total_size / 2;

    int i, j, k, l;
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            for (k = i - half, arr_i = 0; k <= i + half; k++) {
                int x = symmetry(k, ROW);
                for (l = j - half; l <= j + half; l++, arr_i++) {
                    int y = symmetry(l, COL);
                    arr[arr_i] = in[x][y];
                }
            }
            qsort(arr, total_size, sizeof(*arr), cmp);
            out[i][j] = arr[median];
        }
    }
    free(arr);
}

void threshold_filter(unsigned char (*out)[COL], unsigned char (*in)[COL])
{
    int i, j;
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            if (in[i][j] > 120)
                out[i][j] = 255;
            else
                out[i][j] = 0;
        }
    }
}

void threshold_filter_tr(unsigned char (*out)[COL_TR],
        unsigned char (*in)[COL_TR])
{
    int i, j;
    for (i = 0; i < ROW_TR; i++) {
        for (j = 0; j < COL_TR; j++) {
            if (in[i][j] > 200)
                out[i][j] = 255;
            else
                out[i][j] = 0;
        }
    }
}

void process_tt(unsigned char (*in1)[COL], unsigned char (*in2)[COL])
{
    int num = 0;
    int flag = 0;
    int i, j;
    unsigned char (*tmp)[COL] = in1;
    for (int x = 0; x < 2; ++x) {
        if (x == 1)
            tmp = in2;
        for (j = 0; j < COL; ++j) {
            for (i = 0; i < ROW; ++i) {
                if (tmp[i][j] == 0)
                    break;
            }
            if (i != ROW) {
                if (flag == 0) {
                    test_c[num].w_start = j;
                    flag = 1;
                }
            } else {
                if (flag == 1) {
                    test_c[num].w_end = j - 1;
                    num++;
                    flag = 0;
                }
            }
        }
    }
    tmp = in1;
    for (int x = 0; x < num; ++x) {
        if (x == 5)
            tmp = in2;
        for (i = 0; i < ROW; ++i) {
            for (j = test_c[x].w_start; j <= test_c[x].w_end; ++j) {
                if (tmp[i][j] == 0) {
                    test_c[x].h_start = i;
                    goto label1;
                }
            }
        }
label1:
        for (i = ROW - 1; i >= 0; --i) {
            for (j = test_c[x].w_start; j <= test_c[x].w_end; ++j) {
                if (tmp[i][j] == 0) {
                    test_c[x].h_end = i;
                    goto label2;
                }
            }
        }
label2: ;
    }
    tmp = in1;
    for (int x = 0; x < num; ++x) {
        if (x == 5)
            tmp = in2;
        for (j = test_c[x].w_start; j <= test_c[x].w_end; ++j) {
            tmp[test_c[x].h_start][j] = 128;
            tmp[test_c[x].h_end][j] = 128;
        }
        for (i = test_c[x].h_start; i <= test_c[x].h_end; ++i) {
            tmp[i][test_c[x].w_start] = 128;
            tmp[i][test_c[x].w_end] = 128;
        }
    }
}

void process_tr(unsigned char (*in)[COL_TR])
{
    int slice_num = 0;
    int i, j;
    int flag = 0;
    int num = 0;
    for (i = 0; i < ROW_TR; ++i) {
        for (j = 0; j < COL_TR; ++j) {
            if (in[i][j] == 0)
                break;
        }
        if (j != COL_TR) {
            if (flag == 0) {
                slices[slice_num].h_start = i;
                flag = 1;
            }
        } else {
            if (flag == 1) {
                slices[slice_num].h_end = i - 1;
                slice_num++;
                flag = 0;
            }
        }
    }
    for (int x = 0; x < slice_num; ++x) {
        for (j = 0; j < COL_TR; ++j) {
            for (i = slices[x].h_start; i <= slices[x].h_end; ++i) {
                if (in[i][j] == 0)
                    break;
            }
            if (i <= slices[x].h_end) {
                if (flag == 0) {
                    train_c[num].w_start = j;
                    flag = 1;
                }
            } else {
                if (flag == 1) {
                    train_c[num].w_end = j - 1;
                    train_c[num].h_start = slices[x].h_start;
                    train_c[num].h_end = slices[x].h_end;
                    num++;
                    flag = 0;
                }
            }
        }
    }
    for (int x = 0; x < num; ++x) {
        for (i = train_c[x].h_start; i <= train_c[x].h_end; ++i) {
            for (j = train_c[x].w_start; j <= train_c[x].w_end; ++j) {
                if (in[i][j] == 0) {
                    train_c[x].h_start = i;
                    goto label3;
                }
            }
        }
label3:
        for (i = train_c[x].h_end; i >= train_c[x].h_start; --i) {
            for (j = train_c[x].w_start; j <= train_c[x].w_end; ++j) {
                if (in[i][j] == 0) {
                    train_c[x].h_end = i;
                    goto label4;
                }
            }
        }
label4: ;
    }
    for (int x = 0; x < num; ++x) {
        for (j = train_c[x].w_start; j <= train_c[x].w_end; ++j) {
            in[train_c[x].h_start][j] = 128;
            in[train_c[x].h_end][j] = 128;
        }
        for (i = train_c[x].h_start; i <= train_c[x].h_end; ++i) {
            in[i][train_c[x].w_start] = 128;
            in[i][train_c[x].w_end] = 128;
        }
    }

}
