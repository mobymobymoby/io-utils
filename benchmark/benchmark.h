#ifndef __BENCHMARK__H
#define __BENCHMARK__H

#include <bits/time.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

__inline__ uint64_t rdtsc(void) {
    uint32_t lo, hi;
    __asm__ __volatile__ (      // serialize
    "xorl %%eax,%%eax \n        cpuid"
    ::: "%rax", "%rbx", "%rcx", "%rdx");
    /* We cannot use "=A", since this would use %rax on x86_64 and return only the lower 32bits of the TSC */
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return (uint64_t)hi << 32 | lo;
}

#define X 1000000000L

typedef struct timer {
    char *name;
    uint64_t begin;
    uint64_t end;

    struct timespec t_begin;
    struct timespec t_end;
} timer;

#define BEGIN(timer, id) do{\
timer.name = #id;\
timer.begin = rdtsc();\
}while(0)

#define END(timer) timer.end = rdtsc();
#define TIME(timer) (timer.end - timer.begin)

#define T_BEGIN(timer, id) do{\
timer.name = #id;\
clock_gettime(CLOCK_MONOTONIC, &timer.t_begin);\
}while(0)

#define T_END(timer) clock_gettime(CLOCK_MONOTONIC, &timer.t_end);
#define T_TIME(timer) (          \
    (X * (timer.t_end.tv_sec - timer.t_begin.tv_sec)) \
    + (timer.t_end.tv_nsec - timer.t_begin.tv_nsec)\
)

#endif
