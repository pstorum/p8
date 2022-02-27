// llist.h

#ifndef P8_H
#define P8_H

struct block {
    struct block *next;
    int size;     // Bytes
    int in_use;   // Boolean
};

void * myalloc(int allo);
void print_data(void);
void myfree(void *p);

#endif