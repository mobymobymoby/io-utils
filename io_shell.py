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
            command = input("io๐พ ")
            command = command.strip()
        except KeyboardInterrupt:
            os.close(fd)
            print("๐พ ๐พ ๐พ ๐พ ๐พ ๐พ ๐พ ๐พ")
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
- ํ์ฑ ๋ถ๋ถ์ด ๋๋ฌด ๋นํจ์จ์?์ธ๋ฐ..
["arg0", "arg1", number, XXX]
์ด๋ฐ ๋ฌธ๋ฒ ๊ธฐ๋ฐ์ผ๋ก ํ์ฑํ? ์ ์์ผ๋ฉด ์ข ๋ ํธํ? ๋ฏ

- gparted ๋ก mount๋์ง ์์ disk ๋ฅผ ์?์?ํ splitํด์ read/write ํ์คํธ
hdd๊ฐ block device ๋ผ์ byte ๋จ์๋ก writeํ๋ ๊ฒ ์๋์ง ์์๊น๋ผ๋ ์๋ฌธ์ด ๋ค์ด์ ํ์คํธํด๋ดค๋๋ฐ

```
write block 0 0
read byte 1 0
write byte 1 0 -1
read byte 1 0
```

/dev/sdbN ์์ ํน์? ๋ฐ์ดํธ๋ง ์์?ํ๋ ๊ฒ๋ ๊ฐ๋ฅํ๊ณ?,
๊ทธ๋ฅ ์ผ๋ฐ ํ์ผ์ฒ๋ผ ์ฝ๊ณ? ์ธ ์ ์๋ ๋ฏ.
fs ๋ด๋ถ์?์ผ๋ก๋ block ๋จ์๋ก ์?๊ทผํ๊ณ? ์๊ฒ?์ง?
- ์.. ์ปค๋์์ device ํ์ผ์ธ์ง ์ผ๋ฐ ํ์ผ์ธ์ง์ ๋ฐ๋ผ ๋ค๋ฅด๊ฒ ๋์ํ?ํ๋ฐ, block device driver ๋จ์์ byte-level access ๋ ์ง์์ ํด์ฃผ๋?
- TODO: device driver ์ฝ๋ ๋ถ์ (character device, block device) 


- fsync ์ํ๊ณ? ๋ฐ๋ก ๋๋ฉด ๋์คํฌ์ ๋ฐ๋ก ๋ฐ์์ด ์๋์ด์๊ฒ?์ง?
ํ์คํธํด๋ณด๊ณ? ์ถ์๋ฐ ๊ทธ๋ฅ write ๋๋์๋ง์ ํ๋ก์ธ์ค ์ข๋ฃ์ํค๋ ๊ฒ์ผ๋ก๋ ๋ถ์ถฉ๋ถํจ. 
์ปค๋์ด ์ฃผ๊ธฐ์?์ผ๋ก ํด์ค๊ฑฐ๊ณ?, ์๋ก ์์ ํ read๋ฅผ ํ๊ฑฐ๋ dd ๋ก ๋คํ๋ฅผ ๋จ๋ฉด ์ปค๋ buffer cache ์ชฝ์์ ๋ฐ๋ก ์ฝ์ ์ ์์ผ๋
writeํ๊ณ? ์ฆ์ reboot ํ์ ๋ฐ๋ ๋ด์ฉ์ด ์?์ฉ๋์ด์์์ง ํ์คํธ.
    - ์?์์?์ธ shutdown ์์๋ ๋์คํฌ์ ๋ฐ์๋? ์๊ฐ์ด ์ถฉ๋ถํ ๋ฏ..
    - ๊ฐ์? shutdown ํ์คํธ ๊ฒฐ๊ณผ ๋ถํ ์์ journaling ๊ด๋?จ ๋ก๊ทธ๊ฐ ๋จ๊ณ?, ์์ฝ๊ฒ๋ disk์๋ ์?๋๋ก ์๋ฐ์ดํธ๊ฐ ๋์ด ์์
        sudo dd if=/dev/sdb4 of=dump bs=1 count=4K && xxd dump
        - /dev/sdb4๋ fs ๊ฐ ์์ผ๋ journaling ์ํ๋ ๊ฒ ๋ง๊ณ?
"""
