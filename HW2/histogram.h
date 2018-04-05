#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "defs.h"

void histogram(uchar (*data)[COL], const char *str);
void cdf(uchar (*data)[COL], const char *str);

#endif
