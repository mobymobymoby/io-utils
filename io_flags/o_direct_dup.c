#define _GNU_SOURCE
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <inttypes.h>

#define X(ptr) ((unsigned long long)(ptr))
#define ALIGN(ptr, by) (void *)(X((char *)ptr + (by) - 1) & ~((by) - 1))
#define PATH "/dev/sdb4"
#define SECTOR_SIZE 512
#define PAGE 4096

void reader(int fd, int offset, int size, char *output) {
    printf("[!] read, fd: %d, offset: %d, size: %d\n", fd, offset, size);
    int res = pread(fd, output, size, offset); 
    if (res != size) {
        printf("pread\n");
        exit(1);
    }

    return;
}

void writer(int fd, int offset, int size, char *input) {
    printf("[!] write, fd: %d, offset: %d, size: %d\n", fd, offset, size);
    int res = pwrite(fd, input, size, offset);
    if (res != size) {
        printf("pwrite\n");
        exit(1);
    }
}

void hexdump(char *buf, int size) {
    int i;
    printf("\nbegin [ %p ]\n", buf);
    for (i = 0; i < size; i++) {
        printf("%02hhX ", buf[i]);
        if (i % 32 == 31) printf("\n");
    }
    printf("[ %p ] end\n", buf);
}

int main() {
    int fd_r, fd_direct_rw;
    char *buf, *buf_direct, *buf_ptr;
    fd_r = open(PATH, O_RDONLY);
    fd_direct_rw = open(PATH, O_RDWR | O_DIRECT);

    if (fd_direct_rw < 0 || fd_r < 0) {
        printf("open\n");
        return 0;
    }

    buf = malloc(PAGE);
    buf_ptr = malloc(PAGE + PAGE - 1);
    buf_direct = ALIGN(buf_ptr, PAGE);
    
    assert(((uint64_t)buf_direct % PAGE) == 0);
    
    printf("buf: %p\n", buf);
    printf("buf_direct: %p\n", buf_direct);

    memset(buf, 0, PAGE);
    memset(buf_direct, 0, PAGE);

    reader(fd_r, 0, SECTOR_SIZE, buf);
    reader(fd_direct_rw, 0, SECTOR_SIZE, buf_direct);

    hexdump(buf, SECTOR_SIZE);
    hexdump(buf_direct, SECTOR_SIZE);

    memset(buf_direct, 0xee, SECTOR_SIZE);
    writer(fd_direct_rw, 0, SECTOR_SIZE, buf_direct);

    reader(fd_r, 0, SECTOR_SIZE, buf);
    hexdump(buf, SECTOR_SIZE);
    hexdump(buf_direct, SECTOR_SIZE);
    
    memset(buf_direct, 0x00, SECTOR_SIZE);
    writer(fd_direct_rw, 0, SECTOR_SIZE, buf_direct);
    free(buf);
    free(buf_ptr);
}