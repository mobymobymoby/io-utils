# simple benchmark
disk 에서 4MB 데이터를 읽어올 때, 읽어들이는 순서에 따라 성능 비교
- 마운트되어있지 않은 디바이스로 테스트

## default

### hdd
```bash
gcc test.c -O -g && sudo ./a.out ??
fd: 3
read_disk
391050612
read_disk_by_block_2
338576706
read_disk_by_block
328194626
rev_read_disk_by_block
1380268114
zigzag_read_disk_by_block
446042038
random_read_disk_by_block
1353263342
```

### ssd
```bash
$ gcc -O test.c && sudo ./a.out /dev/nvme0n1p3
fd: 3
read_disk
89939171
read_disk_by_block_2
89043762
read_disk_by_block
95398844
rev_read_disk_by_block
200272871
zigzag_read_disk_by_block
81028977
random_read_disk_by_block
203763949
```

## O_DIRECT

### hdd
```bash
gcc test.c -O -g -DDIRECT && sudo ./a.out ??
fd: 3
read_disk
338719894
read_disk_by_block_2
750469758
read_disk_by_block
386980886
rev_read_disk_by_block
1290160726
zigzag_read_disk_by_block
1295194300
random_read_disk_by_block
1546415232
```
### ssd
```bash
$ gcc -O -DDIRECT test.c && sudo ./a.out /dev/nvme0n1p3
fd: 3
read_disk
60673211
read_disk_by_block_2
93690113
read_disk_by_block
119156291
rev_read_disk_by_block
189165760
zigzag_read_disk_by_block
180739825
random_read_disk_by_block
199988322
```

- O_DIRECT 줬을 때는 그냥 연속된 블럭을 한번에 읽을 때 성능이 최대
- 연속적인 블럭이 아니면 더 적은 양의 데이터를 읽더라도 성능이 매우 떨어짐
