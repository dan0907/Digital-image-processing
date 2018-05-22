#ifndef DISJOINT_SET_H
#define DISJOINT_SET_H

struct disjoint_set;

struct disjoint_set *ds_new(void);

void ds_delete(struct disjoint_set *ptr);

int ds_add(struct disjoint_set *ptr);

int ds_find(struct disjoint_set *ptr, int x);

void ds_union(struct disjoint_set *ptr, int x, int y);

int ds_get_set_num(struct disjoint_set *ptr);

int ds_get_num(struct disjoint_set *ptr);

#endif
