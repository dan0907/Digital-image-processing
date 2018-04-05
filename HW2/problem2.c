#include "file_utils.h"
#include <stdio.h>

void edge_crisp(unsigned char (*out)[COL], const unsigned char (*in)[COL]);
void warp(unsigned char (*out)[COL], const unsigned char (*in)[COL]);

int main(void)
{
    unsigned char s3[ROW][COL];
    unsigned char C[ROW][COL];
    unsigned char D[ROW][COL];

    open_and_read(s3, "sample3.raw");

    edge_crisp(C, (const unsigned char (*)[COL])s3);
    warp(D, (const unsigned char (*)[COL])C);

    open_and_write(C, "C.raw");
    open_and_write(D, "D.raw");

    return 0;
}


void edge_crisp(unsigned char (*out)[COL], const unsigned char (*in)[COL])
{
}

void warp(unsigned char (*out)[COL], const unsigned char (*in)[COL])
{
}
