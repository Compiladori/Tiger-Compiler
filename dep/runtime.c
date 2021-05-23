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
long long *allocRecord(int size) {
    int i;
    int *p, *a;
    p = a = (long long *)malloc(size);
    for ( i = 0; i < size; i += sizeof(long long) ) *p++ = 0;
    return a;
}

void printi(int k) {
    printf("%d", k);
}

long long *initArray(int size, int init) {
    long long i;
    long long *a = (long long *)malloc(size * sizeof(long long));
    for ( i = 0; i < size; i++ ) a[i] = init;
    return a;
}
int main() {
    printf("\n");
    return tigermain(0 /* static link */);
}