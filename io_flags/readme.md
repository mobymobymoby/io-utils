# 리눅스 I/O flag 테스트

## flags
### O_DIRECT
- OS 의 cache layer 를 거치지 않고 device 에 대해 direct I/O를 수행함
- 일반적으로는 (당연히) 성능이 좋지 않음. DBMS 정도의 프로그램에서 직접 I/O를 관리하기 위한 특수한 목적으로 사용
- I/O에 사용되는 **포인터, 사이즈, 오프셋** 모두 sector size(512)에 align 되어있어야 함

# Reference
- [** About Linux FS operations](https://showerbugs.github.io/books/linux/2018-02-09-%EB%A6%AC%EB%88%85%EC%8A%A4%20%EC%8B%9C%EC%8A%A4%ED%85%9C%20%ED%94%84%EB%A1%9C%EA%B7%B8%EB%9E%98%EB%B0%8D%20%EC%B1%95%ED%84%B0%202#)
- [How to get disk sector size?](https://unix.stackexchange.com/questions/2668/finding-the-sector-size-of-a-partition)
- [AIO with O_DIRECT](https://stackoverflow.com/questions/55447218/what-does-o-direct-512-byte-aligned-mean)
