#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ll long long

void print(char* p) {
    printf(p);
}

ll size(char* s) {
    return strlen(s);
}

ll ord(char* s){
    if(strlen(s)) return (int)s[0];
	return -1;
}

char* chr(ll i){
    if(i < 0 || i >= 256){
		printf("chr(%lld) out of range\n", i);
		exit(1);
    }
    char* t = (char*) malloc(2 * sizeof(char));
    t[0] = (char)i;
    t[1] = 0;
    return t;
}

ll *allocRecord(ll size) {
    ll i, *p, *a;
    p = a = (ll*) malloc(size);
    for(i = 0; i < size; i += sizeof(ll)){
        *p++ = 0;
    }
    return a;
}

void printi(ll k) {
    printf("%lld", k);
}

ll *initArray(ll size, ll init) {
    ll i, *a = (ll*)malloc(size * sizeof(ll));
    for(i = 0; i < size; i++){
        a[i] = init;
    }
    return a;
}

ll not(ll i){
    return !i;
}

char* substring(char* s, ll first, ll n){
    ll slen = strlen(s);
    if (first < 0 || first + n > slen) {
		printf("substring([%s],%lld,%lld) out of range\n", s, first, n);
		exit(1);
    }

    char* t = malloc(sizeof(char) * (n + 1));
    t[n] = 0;
    int i;
    for (i = 0; i < n; i++){
        t[i] = s[first + i];
    }
    return t;
}

char* concat(char* a, char* b){
    ll alen = size(a), blen = size(b);
    if (alen == 0) return b;
	if (blen == 0) return a;
    
    long i, n = alen + blen;
    char* t = malloc(sizeof(char) * (n + 1));
    t[n] = 0;

    for (i = 0; i < alen; i++){
        t[i] = a[i];
    }

    for (i = 0; i < blen; i++){
        t[i + alen] = b[i];
    }

    return t;
}

int main() {
    printf("\n");
    return tigermain(0 /* static link */);
}