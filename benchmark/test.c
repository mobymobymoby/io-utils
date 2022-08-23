#define _GNU_SOURCE
#include "benchmark.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <assert.h>
#include <string.h>

#define BLOCK_SIZE (32 * 1024)
#define PAGE_SIZE (4 * 1024 * 1024)
#define BLOCK_CNT (PAGE_SIZE / BLOCK_SIZE)
#define SECTOR_SIZE 4096
#define ITER 10

int fd;
char buf[PAGE_SIZE] __attribute__ ((aligned (SECTOR_SIZE)));
char buf_block[PAGE_SIZE] __attribute__ ((aligned (SECTOR_SIZE)));
char buf_rev[PAGE_SIZE] __attribute__ ((aligned (SECTOR_SIZE)));
char buf_zigzag[PAGE_SIZE] __attribute__ ((aligned (SECTOR_SIZE)));
char buf_random[PAGE_SIZE] __attribute__ ((aligned (SECTOR_SIZE)));
char buf_x[PAGE_SIZE] __attribute__ ((aligned (SECTOR_SIZE)));
int seq[] = {
    8, 105, 116, 3, 50, 25, 41, 35, 
    20, 57, 53, 4, 119, 79, 123, 102, 
    51, 112, 30, 96, 38, 31, 58, 98, 
    74, 12, 22, 92, 90, 68, 36, 11, 5, 
    26, 1, 49, 40, 101, 13, 14, 82, 28,
    27, 110, 71, 109, 77, 67, 104, 76, 
    108, 95, 56, 73, 47, 106, 125, 72, 
    16, 84, 17, 127, 59, 42, 23, 15, 60, 
    7, 54, 121, 52, 64, 63, 43, 6, 18, 
    120, 37, 99, 0, 46, 29, 115, 39, 10,
    93, 78, 88, 85, 19, 55, 75, 81, 87, 
    113, 21, 34, 122, 97, 94, 2, 70, 44,
    83, 69, 124, 91, 24, 48, 33, 126, 
    111, 66, 117, 80, 45, 107, 65, 86, 
    32, 118, 103, 62, 89, 114, 100, 61, 9
};

char *path;
uint64_t res;

// sudo dd if=/dev/urandom of=/dev/sdb4 bs=4M count=1
void open_file() {
    fd = open(path, O_RDWR 
    #ifdef DIRECT 
              | O_DIRECT
    #endif
    );
    printf("fd: %d\n", fd);
    if (fd < 0) exit(1);
}

void flush() {
    // return;
    sync();
    int f = open("/proc/sys/vm/drop_caches", O_WRONLY);
    (void)write(f, "1", 1);
    close(f);
}

void read_disk() {
    res = pread(fd, buf, PAGE_SIZE, 0);
    if (res != PAGE_SIZE)
        exit(1);
}

void read_disk_by_block() {
    int i;
    for (i = 0; i < BLOCK_CNT; i+=1) {
        uint64_t x = (BLOCK_SIZE * i);
        res = pread(fd, buf_block + x, BLOCK_SIZE, x);
        if (res != BLOCK_SIZE)
            exit(1);
    }
}

void read_disk_by_block_2() {
    int i;
    for (i = 0; i < BLOCK_CNT; i+=2) {
        uint64_t x = (BLOCK_SIZE * i);
        res = pread(fd, buf_x + x, BLOCK_SIZE, x);
        if (res != BLOCK_SIZE)
            exit(1);
    }
}


void rev_read_disk_by_block() {
    int i;
    for (i = BLOCK_CNT-1; i >= 0; i-=1) {
        uint64_t x = (BLOCK_SIZE * i);
        res = pread(fd, buf_rev + x, BLOCK_SIZE, x);
        if (res != BLOCK_SIZE)
            exit(1);
    }
}

void zigzag_read_disk_by_block() {
    int i, j;
    int iter = BLOCK_CNT / 2;
    for (i = 0; i < iter; i++) {
        for (j = 0; j < 2; j++) {
            uint64_t x = (i + iter * j) * BLOCK_SIZE;
            res = pread(fd, buf_zigzag + x, BLOCK_SIZE, x);
            if (res != BLOCK_SIZE)
                exit(1);
        }
    }
}

void random_read_disk_by_block() {
    int i;
    for (i = 0; i < BLOCK_CNT; i++)
    {
        uint64_t x = (seq[i]) * BLOCK_SIZE;
        res = pread(fd, buf_random + x, BLOCK_SIZE, x);
        if (res != BLOCK_SIZE)
            exit(1);
    }
}


int main(int argc, char *argv[]) {
    if (argc != 2){
        printf("usage\n");
        return -1;
    }

    path = argv[1];

    timer x;
    int i;
    uint64_t t = 0;
    open_file();
    read_disk();

    printf("read_disk\n");
    for (i = 0; i < ITER; i++) {
        flush();
        BEGIN(x, BEFORE);
        read_disk();
        END(x);
        t += TIME(x);
    }
    printf("%lu\n", t);
    t = 0;

    printf("read_disk_by_block_2\n");
    for (i = 0; i < ITER; i++) {
        flush();
        BEGIN(x, BEFORE);
        read_disk_by_block_2();
        END(x);
        t += TIME(x);
    }
    printf("%lu\n", t);
    t = 0;

    printf("read_disk_by_block\n");
    for (i = 0; i < ITER; i++) {
        flush();
        BEGIN(x, BEFORE);
        read_disk_by_block();
        END(x);
        t += TIME(x);
    }
    printf("%lu\n", t);
    t = 0;

    printf("rev_read_disk_by_block\n");
    for (i = 0; i < ITER; i++) {
        flush();
        BEGIN(x, BEFORE);
        rev_read_disk_by_block();
        END(x);
        t += TIME(x);
    }
    printf("%lu\n", t);
    t = 0;

    printf("zigzag_read_disk_by_block\n");
    for (i = 0; i < ITER; i++) {
        flush();
        BEGIN(x, BEFORE);
        zigzag_read_disk_by_block();
        END(x);
        t += TIME(x);
    }
    printf("%lu\n", t);
    t = 0;

    printf("random_read_disk_by_block\n");
    for (i = 0; i < ITER; i++) {
        flush();
        BEGIN(x, BEFORE);
        random_read_disk_by_block();
        END(x);
        t += TIME(x);
    }
    printf("%lu\n", t);
    
    assert(memcmp(buf, buf_block, PAGE_SIZE) == 0);
    assert(memcmp(buf, buf_rev, PAGE_SIZE) == 0);
    assert(memcmp(buf, buf_zigzag, PAGE_SIZE) == 0);
    assert(memcmp(buf, buf_random, PAGE_SIZE) == 0);

    // printf("clocck_gettime: read_disk_by_block\n");
    // for (i = 0; i < ITER; i++) {
    //     flush();
    //     T_BEGIN(x, BEFORE);
    //     read_disk_by_block();
    //     T_END(x);
    //     printf("%20ld\n", T_TIME(x));
    // }

    // printf("clocck_gettime: read_disk\n");
    // for (i = 0; i < ITER; i++) {
    //     flush();
    //     T_BEGIN(x, BEFORE);
    //     read_disk();
    //     T_END(x);
    //     printf("%20ld\n", T_TIME(x));
    // }

    close(fd);
}
