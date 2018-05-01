#include "disjoint_set.h"
#include <stdlib.h>

struct element {
    int rank;
    int parent;
};

struct disjoint_set {
    int num;
    int set_num;
    struct element *elements;
    int cap;
};

struct disjoint_set *ds_new(void)
{
    struct disjoint_set *ptr = malloc(sizeof(*ptr));
    ptr->num = 0;
    ptr->set_num = 0;
    ptr->cap = 2;
    ptr->elements = malloc(sizeof(*ptr->elements) * ptr->cap);
    return ptr;
}

void ds_delete(struct disjoint_set *ptr)
{
    free(ptr->elements);
    free(ptr);
}

/* return the index(from 0 to n-1) of the added element */
int ds_add(struct disjoint_set *ptr)
{
    if (ptr->num == ptr->cap) {
        ptr->cap *= 2;
        ptr->elements = realloc(ptr->elements,
                sizeof(*ptr->elements) * ptr->cap);
    }
    ptr->elements[ptr->num].rank = 0;
    ptr->elements[ptr->num].parent = ptr->num;
    ptr->set_num++;
    return ptr->num++;
}

int ds_find(struct disjoint_set *ptr, int x)
{
    if (ptr->elements[x].parent == x)
        return x;
    ptr->elements[x].parent = ds_find(ptr, ptr->elements[x].parent);
    return ptr->elements[x].parent;
}

void ds_union(struct disjoint_set *ptr, int x, int y)
{
    int x_root = ds_find(ptr, x);
    int y_root = ds_find(ptr, y);
    if (x_root == y_root)
        return;
    if (ptr->elements[x_root].rank < ptr->elements[y_root].rank) {
        ptr->elements[x_root].parent = y_root;
    } else {
        ptr->elements[y_root].parent = x_root;
        if (ptr->elements[x_root].rank == ptr->elements[y_root].rank)
            ptr->elements[x_root].rank++;
    }
    ptr->set_num--;
}

int ds_get_set_num(struct disjoint_set *ptr)
{
    return ptr->set_num;
}

int ds_get_num(struct disjoint_set *ptr)
{
    return ptr->num;
}

