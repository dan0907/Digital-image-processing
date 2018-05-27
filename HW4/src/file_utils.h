#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include "defs.h"

void open_and_read(unsigned char (*data)[COL], const char *str);

void open_and_write(unsigned char (*data)[COL], const char *str);

void open_and_read_tr(unsigned char (*data)[COL_TR], const char *str);

void open_and_write_tr(unsigned char (*data)[COL_TR], const char *str);

#endif
