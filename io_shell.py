#!/usr/bin/env python3
import os
import sys
from code import interact

BLK = 4096

def num(n: str):
    try:
        if n.startswith("0x"):
            v = int(n[2:], 16)
        else:
            v = int(n)
    except:
        v = 0
    return v

def get_size(fd):
    disk_size = os.lseek(fd, 0, os.SEEK_END)
    print(f"disk size: {hex(disk_size)}")



def main():

    if len(sys.argv) != 2:
        print(f"usage: {sys.argv[0]} <dev_path>")
        exit(1)
    path = sys.argv[1]

    try:
        print(f"open '{path}'")
        fd = os.open(path, os.O_RDWR)
    except PermissionError:
        print("Try again with enough permission")
        exit(1)

    print(f"{path}: {fd}")

    get_size(fd)

    while True:
        try:
            command = input("io💾 ")
            command = command.strip()
        except KeyboardInterrupt:
            os.close(fd)
            print("💾 💾 💾 💾 💾 💾 💾 💾")
            break
        if not command:
            continue
        
        commands = command.split()
        
        if commands[0] == "shell":
            try:
                interact(local = locals())
            except SystemExit:
                pass
        elif commands[0] == "info":
            print(f"{path}: {fd}")
            get_size(fd)
        
        elif commands[0] == "read" and len(commands) > 1:
            """
            read byte count offset
            read block blockno
            """
            if commands[1] == "byte" and len(commands) == 4:
                count = num(commands[2])
                offset = num(commands[3])

                data = os.pread(fd, count, offset)
                if len(data) != count:
                    print("short count")
                print(data)

            elif commands[1] == "block" and len(commands) == 3:
                blockno = num(commands[2])

                data = os.pread(fd, BLK, BLK * blockno)
                if len(data) != BLK:
                    print("short count")
                print(data)

            else:
                print("wrong usage")

        elif commands[0] == "write" and len(commands) > 1:
            """
            write byte count offset [0 | -1]
            write block blockno [0 | -1]
            """
            if commands[1] == "byte" and len(commands) == 5:
                count = num(commands[2])
                offset = num(commands[3])
                if (commands[4] == "0"):
                    data = b"\x00" * count
                else:
                    data = b"\xff" * count
                res = os.pwrite(fd, data, offset)
                if res != count:
                    print("short count")

            elif commands[1] == "block" and len(commands) == 4:
                blockno = num(commands[2])
                if commands[3] == "0":
                    data = b"\x00" * BLK
                else:
                    data = b"\xff" * BLK
                res = os.pwrite(fd, data, blockno * BLK)
                if res != BLK:
                    print("short count")

            else:
                print("wrong usage")

        elif commands[0] == "fsync":
            """
                fsync
            """
            os.fsync(fd)
            print(f"fsync done")

        else:
            print(f"Unknown command: {command}")
        
if __name__ == "__main__":
    main()

"""
- 파싱 부분이 너무 비효율적인데..
["arg0", "arg1", number, XXX]
이런 문법 기반으로 파싱할 수 있으면 좀 더 편할 듯

- gparted 로 mount되지 않은 disk 를 적절히 split해서 read/write 테스트
hdd가 block device 라서 byte 단위로 write하는 게 안되지 않을까라는 의문이 들어서 테스트해봤는데

```
write block 0 0
read byte 1 0
write byte 1 0 -1
read byte 1 0
```

/dev/sdbN 에서 특정 바이트만 수정하는 것도 가능하고,
그냥 일반 파일처럼 읽고 쓸 수 있는 듯.
fs 내부적으로는 block 단위로 접근하고 있겠지?

- fsync 안하고 바로 끄면 디스크에 바로 반영이 안되어있겠지?
테스트해보고 싶은데 그냥 write 끝나자마자 프로세스 종료시키는 것으로는 불충분함. 
커널이 주기적으로 해줄거고, 새로 시작 후 read를 하거나 dd 로 덤프를 뜨면 커널 buffer cache 쪽에서 바로 읽을 수 있으니
write하고 즉시 reboot 후에 바뀐 내용이 적용되어있을지 테스트.
    - 정상적인 shutdown 시에는 디스크에 반영될 시간이 충분한 듯..
    - 강제 shutdown 테스트 결과 부팅 시에 journaling 관련 로그가 뜨고, 아쉽게도 disk에는 제대로 업데이트가 되어 있음
        sudo dd if=/dev/sdb4 of=dump bs=1 count=4K && xxd dump
        - /dev/sdb4는 fs 가 없으니 journaling 안하는 게 맞고
"""
