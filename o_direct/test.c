#define _GNU_SOURCE
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#define SECTOR_SIZE 512
#define X(ptr) ((unsigned long long)(ptr))
#define ALIGN(ptr, by) (void *)(X((char *)ptr + (by) - 1) & ~((by) - 1))

int main() {
    // 초기에 test 파일 생성
    // dd if=/dev/zero of=./test bs=1M count=1
    int fd, res, i;
    char *buf, *a_buf, *msg = "hello! ";
    srand(time(NULL));
    fd = open("./test", O_RDWR | O_DIRECT);
    printf("%d\n", fd);
    
    // buf 주소 랜덤으로 할당하기 위함 -> test 파일의 내용이 실행할 때마다 바뀜
    malloc(rand() % SECTOR_SIZE);

    buf = (char *)malloc(SECTOR_SIZE + SECTOR_SIZE - 1);
    a_buf = ALIGN(buf, SECTOR_SIZE);
    assert(X(a_buf) % SECTOR_SIZE == 0);

    for (i = 0; i < (SECTOR_SIZE*2-1) / strlen(msg); i++) {
        memcpy(buf + i * strlen(msg), msg, strlen(msg));
    }

    res = pwrite(fd, buf, SECTOR_SIZE, 0);
    printf("non aligned ptr: %d\n", res);

    res = pwrite(fd, a_buf, SECTOR_SIZE + 1, 0);
    printf("non aligned size: %d\n", res);

    res = pwrite(fd, a_buf, SECTOR_SIZE, SECTOR_SIZE + 1);
    printf("non aligned offset: %d\n", res);
    
    // O_DIRECT 로 연 파일은 IO 할 때에 ptr, size, offset 모두 디바이스의 섹터 사이즈의 배수가 되어야 함
    res = pwrite(fd, a_buf, SECTOR_SIZE, 0);
    printf("totally aligned: %d\n", res);
}
