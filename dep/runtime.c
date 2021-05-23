#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void print(char *p) {
    printf(p);
}

int size(char *s) {
    int c = 0;
    while ( *s ) {
        c++;
        s++;
    }
    return c;
}
int *allocRecord(int size) {
    int i;
    int *p, *a;
    p = a = (int *)malloc(size);
    for ( i = 0; i < size; i += sizeof(int) ) *p++ = 0;
    return a;
}

void printi(int k) {
    printf("%d", k);
}

int *initArray(int size, int init) {
    int i;
    int *a = (int *)malloc(size * sizeof(int));
    for ( i = 0; i < size; i++ ) a[i] = init;
    return a;
}
int main() {
    printf("\n");
    return tigermain(0 /* static link */);
}