#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "defs.h"

void histogram(unsigned char (*data)[COL], const char *str);
void cdf(unsigned char (*data)[COL], const char *str);

#endif
