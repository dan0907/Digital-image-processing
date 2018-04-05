#include "boundary.h"

/* odd */
int symmetry(int i, int n)
{
    if (i < 0)
        return -i;
    else if (i >= n)
        return 2 * n - i - 2;
    else
        return i;
}
