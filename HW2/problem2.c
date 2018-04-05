#include "file_utils.h"
#include <stdio.h>

void edge_crispening(uchar (*out)[COL], const uchar (*in)[COL]);
void warping(uchar (*out)[COL], const uchar (*in)[COL]);

int main(void)
{
    uchar s3[ROW][COL];
    uchar C[ROW][COL];
    uchar D[ROW][COL];

    open_and_read(s3, "sample3.raw");

    edge_crispening(C, (const uchar (*)[COL])s3);
    warping(D, (const uchar (*)[COL])C);

    open_and_write(C, "C.raw");
    open_and_write(D, "D.raw");

    return 0;
}


void edge_crispening(uchar (*out)[COL], const uchar (*in)[COL])
{
}

void warping(uchar (*out)[COL], const uchar (*in)[COL])
{
}
