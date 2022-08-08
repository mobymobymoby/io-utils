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
파싱 부분이 너무 비효율적인데..
["arg0", "arg1", number, XXX]
이런 문법 기반으로 파싱할 수 있으면 좀 더 편할 듯
"""
