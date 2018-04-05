#include "file_utils.h"
#include "boundary.h"
#include "histogram.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

void first_order_edge(uchar (*out)[COL], const uchar (*in)[COL]);
void second_order_edge(uchar (*out)[COL], const uchar (*in)[COL]);
void canny_edge(uchar (*out)[COL], const uchar (*in)[COL]);
void process(uchar (*out)[COL], const uchar (*in)[COL]);


int main(void)
{
    uchar s1[ROW][COL];
    uchar s2[ROW][COL];
    uchar a1[ROW][COL];
    uchar a2[ROW][COL];
    uchar a3[ROW][COL];
    uchar b[ROW][COL];

    open_and_read(s1, "sample1.raw");
    open_and_read(s2, "sample2.raw");

    first_order_edge(a1, (const uchar (*)[COL])s1);
    second_order_edge(a2, (const uchar (*)[COL])s1);
    canny_edge(a3, (const uchar (*)[COL])s1);
    process(b, (const uchar (*)[COL])s2);


    open_and_write(a1, "a1.raw");
    open_and_write(a2, "a2.raw");
    open_and_write(a3, "a3.raw");
    open_and_write(b, "b.raw");

    return 0;
}

void first_order_edge(uchar (*out)[COL], const uchar (*in)[COL])
{
    int side_length = 3;
    int half = side_length / 2;
    int total_size = side_length * side_length;
    uchar *arr = malloc(sizeof(uchar) * total_size);
    int arr_i;
    int i, j, k, l;
    int z = 1;
    uchar threshold = 41;
    for (i = 0; i < COL; i++) {
        for (j = 0; j < COL; j++) {
            int gr, gc;
            uchar tmp;
            for (k = i - half, arr_i = 0; k <= i + half; k++) {
                int x = symmetry(k, COL);
                for (l = j - half; l <= j + half; l++, arr_i++) {
                    int y = symmetry(l, COL);
                    arr[arr_i] = in[x][y];
                }
            }
            gr = (arr[2] + z * arr[5] + arr[8] - arr[0]
                    - z * arr[3] - arr[6]) / (z + 2);
            gc = (arr[0] + z * arr[1] + arr[2] - arr[6]
                    - z * arr[7] - arr[8]) / (z + 2);
            tmp = sqrt(pow(gr, 2) + pow(gc, 2));
            if (tmp >= threshold)
                out[i][j] = 255;
            else
                out[i][j] = 0;
        }
    }
    free(arr);
}

void second_order_edge(uchar (*out)[COL], const uchar (*in)[COL])
{
}

void canny_edge(uchar (*out)[COL], const uchar (*in)[COL])
{
}

void process(uchar (*out)[COL], const uchar (*in)[COL])
{
}
