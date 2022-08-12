// preprocessor 동작 확인용
#include <stdio.h>
#define test(name) int test##name = 1000
#define sharp(input) #input

int main() {
    test(123);
    printf("%d\n", test123);
    printf("%s\n", sharp(안녕하세요));
}