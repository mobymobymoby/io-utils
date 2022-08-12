#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#define DIFF(a, b) ((uint64_t)(a) - (uint64_t)(b))

void test() {
    char *str = "1, 2, 3,4  , 0  , 1 , 9\n";
    char *start, *stop, *end = str + strlen(str);
    start = str;
    int n;
    n = strtol(start, &stop, 10);
    printf("%d(%d)(%d)\n", n, *stop, DIFF(stop, start));

    while (1){
        start = stop + 1;
        n = strtol(start, &stop, 10);
        if (DIFF(stop, start))
            printf("%d(%d)(%d)\n", n, *stop, DIFF(stop, start));
        if (stop == end) break;
    }
}

int main() {
    test();
}