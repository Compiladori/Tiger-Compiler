#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void print(char* p )
{
    printf(p);
}

int size(char *s)
{
    int c = 0;
    while(*s) {
        c++;
        s++;
    }
    return c;
}


int main()
{
    return tigermain(0 /* static link */);
}