#include "file_utils.h"
#include "disjoint_set.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    unsigned char s1[ROW][COL];
    unsigned char s2[ROW][COL];
    unsigned char train[ROW_TR][COL_TR];
    open_and_read(s1, "sample1.raw");
    open_and_read(s2, "sample2.raw");
    open_and_read_train(train, "TrainingSet.raw");

    return 0;
}

