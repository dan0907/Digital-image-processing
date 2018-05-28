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
void init(unsigned char (*in1)[COL], unsigned char (*in2)[COL],
        unsigned char (*in3)[COL_TR]);
void cal_ccs_and_holes(void);
void predict(void);

char test_set1[] = {'H', 'i', 'g', 'x', '8'};
char test_set2[] = {'S', 'B', '4', 'T', '7', 'I'};
char train_set[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
    'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '!', '@', '#', '$', '%', '^', '&', '*'
};

struct feature {
    double h_w_ratio;
    int h_num;
    int cc_num;
    int euler_num;
};

struct test_s {
    char real;
    int w_start;
    int w_end;
    int h_start;
    int h_end;
    unsigned char (*im)[COL];
    struct feature f;
};

struct train_s {
    char real;
    char predict;
    int w_start;
    int w_end;
    int h_start;
    int h_end;
    unsigned char (*im)[COL_TR];
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
    unsigned char s1_pre1[ROW][COL]; //final
    unsigned char s2_pre1[ROW][COL];
    unsigned char s2_pre2[ROW][COL];
    unsigned char s2_pre3[ROW][COL]; //final
    unsigned char train[ROW_TR][COL_TR];
    unsigned char train_pre[ROW_TR][COL_TR]; //final
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
    init(s1_pre1, s2_pre3, train_pre);
    cal_ccs_and_holes();
    predict();

    return 0;
}

// compare function for qsort
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
                    goto label1; //break out of the nested loop
                }
            }
        }
label1:
        for (i = ROW - 1; i >= 0; --i) {
            for (j = test_c[x].w_start; j <= test_c[x].w_end; ++j) {
                if (tmp[i][j] == 0) {
                    test_c[x].h_end = i;
                    goto label2; //break out of the nested loop
                }
            }
        }
label2: ;
    }
    tmp = in1;
    // draw bounding box
    for (int x = 0; x < num; ++x) {
        test_c[x].f.h_w_ratio = (double)(test_c[x].h_end-test_c[x].h_start)
            / (test_c[x].w_end-test_c[x].w_start);
        if (x == 5)
            tmp = in2;
        for (j = test_c[x].w_start - 1; j <= test_c[x].w_end + 1; ++j) {
            tmp[test_c[x].h_start-1][j] = 128;
            tmp[test_c[x].h_end+1][j] = 128;
        }
        for (i = test_c[x].h_start - 1; i <= test_c[x].h_end + 1; ++i) {
            tmp[i][test_c[x].w_start-1] = 128;
            tmp[i][test_c[x].w_end+1] = 128;
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
                    goto label3; //break out of the nested loop
                }
            }
        }
label3:
        for (i = train_c[x].h_end; i >= train_c[x].h_start; --i) {
            for (j = train_c[x].w_start; j <= train_c[x].w_end; ++j) {
                if (in[i][j] == 0) {
                    train_c[x].h_end = i;
                    goto label4; //break out of the nested loop
                }
            }
        }
label4: ;
    }
    // draw bounding box
    for (int x = 0; x < num; ++x) {
        train_c[x].f.h_w_ratio = (double)(train_c[x].h_end-train_c[x].h_start)
            / (train_c[x].w_end-train_c[x].w_start);
        for (j = train_c[x].w_start - 1; j <= train_c[x].w_end + 1; ++j) {
            in[train_c[x].h_start-1][j] = 128;
            in[train_c[x].h_end+1][j] = 128;
        }
        for (i = train_c[x].h_start - 1; i <= train_c[x].h_end + 1; ++i) {
            in[i][train_c[x].w_start-1] = 128;
            in[i][train_c[x].w_end+1] = 128;
        }
    }

}
void init(unsigned char (*in1)[COL], unsigned char (*in2)[COL],
        unsigned char (*in3)[COL_TR])
{
    int i;
    for (i = 0; i < 5; ++i) {
        test_c[i].real = test_set1[i];
        test_c[i].im = in1;
    }
    for (i = 0; i < 6; ++i) {
        test_c[i+5].real = test_set2[i];
        test_c[i+5].im = in2;
    }
    for (i = 0; i < 70; ++i) {
        train_c[i].real = train_set[i];
        train_c[i].im = in3;
    }
}
void cal_ccs_and_holes(void)
{
    unsigned char label_cc[ROW_TR][COL_TR];
    unsigned char label_h[ROW_TR][COL_TR];
    for (int x = 0; x < 70; ++x) {
        struct disjoint_set *cc_ds_ptr = ds_new();
        struct disjoint_set *h_ds_ptr = ds_new();
        int i, j;
        /* initialize */
        ds_add(h_ds_ptr);
        for (i = train_c[x].h_start; i <= train_c[x].h_end; i++) {
            for (j = train_c[x].w_start; j <= train_c[x].w_end; j++) {
                label_cc[i][j] = train_c[x].im[i][j] ? 0 : 1;
                label_h[i][j] = train_c[x].im[i][j] ? 1 : 0;
            }
        }

        /* process */
        for (i = train_c[x].h_start; i <= train_c[x].h_end; i++) {
            for (j = train_c[x].w_start; j <= train_c[x].w_end; j++) {
                int num = 0;
                unsigned char tmp[4];
                if (label_cc[i][j]) {
                    if (j-1 >= train_c[x].w_start && label_cc[i][j-1]) {
                        tmp[num++] = label_cc[i][j-1];
                    }
                    if (j-1 >= train_c[x].w_start
                            && i-1 >= train_c[x].h_start
                            && label_cc[i-1][j-1]) {
                        tmp[num++] = label_cc[i-1][j-1];
                    }
                    if (i-1 >= train_c[x].h_start && label_cc[i-1][j]) {
                        tmp[num++] = label_cc[i-1][j];
                    }
                    if (i-1 >= train_c[x].h_start
                            && j+1 <= train_c[x].w_end
                            && label_cc[i-1][j+1]) {
                        tmp[num++] = label_cc[i-1][j+1];
                    }
                    if (num == 0) {
                        label_cc[i][j] = ds_add(cc_ds_ptr) + 1;
                    } else {
                        int k;
                        label_cc[i][j] = tmp[0];
                        for (k = 1; k < num; k++) {
                            ds_union(cc_ds_ptr, tmp[0] - 1, tmp[k] - 1);
                        }
                    }
                } else {
                    if (j-1 >= train_c[x].w_start && label_h[i][j-1]) {
                        tmp[num++] = label_h[i][j-1];
                    }
                    if (j-1 >= train_c[x].w_start
                            && i-1 >= train_c[x].h_start
                            && label_h[i-1][j-1]) {
                        tmp[num++] = label_h[i-1][j-1];
                    }
                    if (i-1 >= train_c[x].h_start && label_h[i-1][j]) {
                        tmp[num++] = label_h[i-1][j];
                    }
                    if (i-1 >= train_c[x].h_start
                            && j+1 <= train_c[x].w_end
                            && label_h[i-1][j+1]) {
                        tmp[num++] = label_h[i-1][j+1];
                    }
                    if (num == 0) {
                        label_h[i][j] = ds_add(h_ds_ptr) + 1;
                    } else {
                        int k;
                        label_h[i][j] = tmp[0];
                        for (k = 1; k < num; k++) {
                            ds_union(h_ds_ptr, tmp[0] - 1, tmp[k] - 1);
                        }
                    }
                    if (i == train_c[x].h_start || i == train_c[x].h_end
                            || j == train_c[x].w_start || j == train_c[x].w_end)
                        ds_union(h_ds_ptr, label_h[i][j] - 1, 0);
                }
            }
        }
        train_c[x].f.cc_num = ds_get_set_num(cc_ds_ptr);
        ds_delete(cc_ds_ptr);
        train_c[x].f.h_num = ds_get_set_num(h_ds_ptr)-1;
        ds_delete(h_ds_ptr);
        printf("%c: cc_num=%d, h_num=%d\n", train_c[x].real,
                train_c[x].f.cc_num, train_c[x].f.h_num);
    }

    for (int x = 0; x < 11; ++x) {
        struct disjoint_set *cc_ds_ptr = ds_new();
        struct disjoint_set *h_ds_ptr = ds_new();
        int i, j;
        /* initialize */
        ds_add(h_ds_ptr);
        for (i = test_c[x].h_start; i <= test_c[x].h_end; i++) {
            for (j = test_c[x].w_start; j <= test_c[x].w_end; j++) {
                label_cc[i][j] = test_c[x].im[i][j] ? 0 : 1;
                label_h[i][j] = test_c[x].im[i][j] ? 1 : 0;
            }
        }

        /* process */
        for (i = test_c[x].h_start; i <= test_c[x].h_end; i++) {
            for (j = test_c[x].w_start; j <= test_c[x].w_end; j++) {
                int num = 0;
                unsigned char tmp[4];
                if (label_cc[i][j]) {
                    if (j-1 >= test_c[x].w_start && label_cc[i][j-1]) {
                        tmp[num++] = label_cc[i][j-1];
                    }
                    if (j-1 >= test_c[x].w_start
                            && i-1 >= test_c[x].h_start
                            && label_cc[i-1][j-1]) {
                        tmp[num++] = label_cc[i-1][j-1];
                    }
                    if (i-1 >= test_c[x].h_start && label_cc[i-1][j]) {
                        tmp[num++] = label_cc[i-1][j];
                    }
                    if (i-1 >= test_c[x].h_start
                            && j+1 <= test_c[x].w_end
                            && label_cc[i-1][j+1]) {
                        tmp[num++] = label_cc[i-1][j+1];
                    }
                    if (num == 0) {
                        label_cc[i][j] = ds_add(cc_ds_ptr) + 1;
                    } else {
                        int k;
                        label_cc[i][j] = tmp[0];
                        for (k = 1; k < num; k++) {
                            ds_union(cc_ds_ptr, tmp[0] - 1, tmp[k] - 1);
                        }
                    }
                } else {
                    if (j-1 >= test_c[x].w_start && label_h[i][j-1]) {
                        tmp[num++] = label_h[i][j-1];
                    }
                    if (j-1 >= test_c[x].w_start
                            && i-1 >= test_c[x].h_start
                            && label_h[i-1][j-1]) {
                        tmp[num++] = label_h[i-1][j-1];
                    }
                    if (i-1 >= test_c[x].h_start && label_h[i-1][j]) {
                        tmp[num++] = label_h[i-1][j];
                    }
                    if (i-1 >= test_c[x].h_start
                            && j+1 <= test_c[x].w_end
                            && label_h[i-1][j+1]) {
                        tmp[num++] = label_h[i-1][j+1];
                    }
                    if (num == 0) {
                        label_h[i][j] = ds_add(h_ds_ptr) + 1;
                    } else {
                        int k;
                        label_h[i][j] = tmp[0];
                        for (k = 1; k < num; k++) {
                            ds_union(h_ds_ptr, tmp[0] - 1, tmp[k] - 1);
                        }
                    }
                    if (i == test_c[x].h_start || i == test_c[x].h_end
                            || j == test_c[x].w_start || j == test_c[x].w_end)
                        ds_union(h_ds_ptr, label_h[i][j] - 1, 0);
                }
            }
        }
        test_c[x].f.cc_num = ds_get_set_num(cc_ds_ptr);
        ds_delete(cc_ds_ptr);
        test_c[x].f.h_num = ds_get_set_num(h_ds_ptr)-1;
        ds_delete(h_ds_ptr);
        printf("%c: cc_num=%d, h_num=%d\n", test_c[x].real,
                test_c[x].f.cc_num, test_c[x].f.h_num);
    }
}
void predict(void)
{
}
