#include "file_utils.h"
#include "disjoint_set.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void boundary_extraction(unsigned char (*out)[COL], unsigned char (*in)[COL]);
int ccl(unsigned char (*in)[COL]);
void skeletonizing(unsigned char (*out)[COL], unsigned char (*in)[COL]);

int main(void)
{
    unsigned char data[ROW][COL];
    unsigned char B[ROW][COL] = {{0}};
    unsigned char S[ROW][COL] = {{0}};
    open_and_read(data, "sample1.raw");

    boundary_extraction(B, data);
    open_and_write(B, "B.raw");

    printf("Number of objects is %d\n", ccl(data));

    skeletonizing(S, data);
    open_and_write(S, "S.raw");

    return 0;
}

void boundary_extraction(unsigned char (*out)[COL], unsigned char (*in)[COL])
{
    int i, j;
    for (i = 1; i < ROW - 1; i++) {
        for (j = 1; j < COL - 1; j++) {
            if (in[i][j] == 0)
                continue;
            out[i][j] = in[i][j] - (in[i - 1][j - 1] & in[i - 1][j]
                    & in[i - 1][j + 1] & in[i][j - 1] & in[i][j + 1]
                    & in[i + 1][j - 1] & in[i + 1][j] & in[i + 1][j + 1]);
        }
    }
}

/* 8-neighbors */
int ccl(unsigned char (*in)[COL])
{
    unsigned char label[ROW][COL];
    struct disjoint_set *ds_ptr = ds_new();
    int i, j;
    int count;
    /* initialize */
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            label[i][j] = in[i][j] ? 1 : 0;
        }
    }

    /* process */
    for (i = 1; i < ROW - 1; i++) {
        for (j = 1; j < COL - 1; j++) {
            int num = 0;
            unsigned char tmp[4];
            if (label[i][j] == 0)
                continue;
            if (label[i][j - 1]) {
                tmp[num++] = label[i][j - 1];
            }
            if (label[i - 1][j - 1]) {
                tmp[num++] = label[i - 1][j - 1];
            }
            if (label[i - 1][j]) {
                tmp[num++] = label[i - 1][j];
            }
            if (label[i - 1][j + 1]) {
                tmp[num++] = label[i - 1][j + 1];
            }
            if (num == 0) {
                label[i][j] = ds_add(ds_ptr) + 1;
            } else {
                int k;
                label[i][j] = tmp[0];
                for (k = 1; k < num; k++) {
                    ds_union(ds_ptr, tmp[0] - 1, tmp[k] - 1);
                }
            }
        }
    }
    count = ds_get_set_num(ds_ptr);
    ds_delete(ds_ptr);
    return count;
}

void skeletonizing(unsigned char (*out)[COL], unsigned char (*in)[COL])
{
    unsigned char (*label_old)[COL];
    unsigned char (*label_new)[COL];
    void *tmp_ptr;
    int i, j;
    int flag;
    label_old = malloc(sizeof(*label_old) * ROW);
    label_new = malloc(sizeof(*label_new) * ROW);
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            label_old[i][j] = in[i][j] ? 1 : 0;
        }
    }
    do {
        flag = 0;
        for (i = 1; i < ROW - 1; i++) {
            for (j = 1; j < COL - 1; j++) {
                int min_value;
                if (label_old[i][j] == 0) {
                    label_new[i][j] = 0;
                    continue;
                }
                min_value = label_old[i - 1][j];
                if (label_old[i][j - 1] < min_value)
                    min_value = label_old[i][j - 1];
                if (label_old[i][j + 1] < min_value)
                    min_value = label_old[i][j + 1];
                if (label_old[i + 1][j] < min_value)
                    min_value = label_old[i + 1][j];
                if (label_old[i][j] != min_value + 1)
                    flag = 1;
                label_new[i][j] = min_value + 1;
            }
        }
        tmp_ptr = label_old;
        label_old = label_new;
        label_new = tmp_ptr;
    } while (flag);

    for (i = 1; i < ROW - 1; i++) {
        for (j = 1; j < COL - 1; j++) {
            int max_value;
            if (label_old[i][j] == 0)
                continue;
            max_value = label_old[i - 1][j];
            if (label_old[i][j - 1] > max_value)
                max_value = label_old[i][j - 1];
            if (label_old[i][j + 1] > max_value)
                max_value = label_old[i][j + 1];
            if (label_old[i + 1][j] > max_value)
                max_value = label_old[i + 1][j];
            if (label_old[i][j] == max_value)
                out[i][j] = 255;
        }
    }
    free(label_old);
    free(label_new);
}
