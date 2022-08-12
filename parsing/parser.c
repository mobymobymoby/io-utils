#include <stdio.h>
#include <string.h>

#define FALSE 0
#define TRUE 1
#define ARR(...) __VA_ARGS__
#define MAX_ARR 1000

char *strings[] = {
    #define format(str, answer, count) str,
    #include "tdd.list"
    #undef format
};

int counts[] = {
    #define format(str, answer, count) count,
    #include "tdd.list"
    #undef format
};

int answers[][MAX_ARR] = {
    #define format(str, answer, count) answer,
    #include "tdd.list"
    #undef format
};

int check(int *my, int my_cnt, int *answer, int answer_cnt) {
    int i;

    if (my_cnt != answer_cnt) 
        return FALSE;

    for (i = 0; i < my_cnt; i++)
        if (my[i] != answer[i]) 
            return FALSE;

    return TRUE;
}

int test(char *str, int answer[], int count) {
    int parsed[MAX_ARR], idx = 0, i;
    int num;

    char *ptr, *stop;
    ptr = str;

    num = strtol(ptr, &stop, 10);
    parsed[idx++] = num;

    



    if (!check(parsed, idx, answer, count)) {
        printf("  Parsed: ");
        for (i = 0; i < idx; i++)   printf("%8d", parsed[i]);
        printf("\n");
        printf("Expected: ");
        for (i = 0; i < count; i++) printf("%8d", answer[i]);
        printf("\n");
        printf("------------------------------------------------------------------\n");
    }
}

int main() {
    int i;
    int num_tests = sizeof(counts) / sizeof(int);
    
    for (i = 0; i < num_tests; i++) {
        test(strings[i], answers[i], counts[i]);
    }
}
