#include "defs.h"
#include "file_utils.h"
#include "boundary.h"

#include <stdio.h>
#include <stdlib.h>

void laws_method(unsigned char (*out)[COL], unsigned char (*in)[COL]);

int main(void)
{
    unsigned char data[ROW][COL];
    unsigned char K[ROW][COL];
    unsigned char L[ROW][COL];

    open_and_read(data, "sample2.raw");

    open_and_write(K, "K.raw");

    open_and_write(L, "L.raw");

    return 0;
}

void laws_method(unsigned char (*out)[COL], unsigned char (*in)[COL])
{
}
