#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_NUM 100
#define SCALE 100000

int test(char *str, int *answer, int count) {
    int arr[MAX_NUM], idx = 0;
    int num, wait_bar = 0;
    char *ptr, *stop, *end;
    ptr = str;
    end = str + strlen(str);

    while (ptr < end) {
        int tmp = strtol(ptr, &stop, 10);
        if (ptr != stop) { // 뭔가 숫자가 들어옴
            if (wait_bar == 0) {
                stop = strstr(stop, "-");
                if (stop == NULL) {
                    break;
                }
                wait_bar = 1;
                num = tmp;
            } else {
                arr[idx++] = num * SCALE + tmp;
                wait_bar = 0;
            }
            // printf("[%c]", *stop);
        }
        ptr = stop + 1;
    }
    if (wait_bar || idx == 0) {
        printf(" no!\n");
        return 0;
    }
    for (int i =0 ;i < idx ; i++)
        printf("%8d ", arr[i]);
    printf("\n");
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    test("1-2,,,,   1-4 , 3-4500 \n", NULL, 0);
    test("0-2,,,,   0-0 , 0-000 \n", NULL, 0);
    test("1-2 1-4 , 3-4500 \n", NULL, 0);
    test("1-2 2-3 3- ,4", NULL, 0);
    test("3 - 4\n", NULL, 0);
    test("3  4\n", NULL, 0);
    test("3 ,,.,..,., 4\n", NULL, 0);
    test("1-2, 2-", NULL, 0);
}
