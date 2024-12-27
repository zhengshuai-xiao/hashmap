./CustomHashMap -me 3 -cb 24 -nb 25 -t 2 -p 64 -mu 32 -s 10 
------------------output-----------------
zxiao@localhost:/mnt/c/workspace/hashmap/build$ ./CustomHashMap -me 3 -cb 24 -nb 25 -t 2 -p 64 -mu 32 -s 10
15
method=3
capBit=24
NodeBit=25
threadNum=2
partitionNum=64
mutexNum=32
starting to generate 2*16777216
data has been initilized, duration:0.475512
using:testCustomHashMapGroupWithMultiThreads
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
created CustomHashMap,capacity:262144, total size:2097152
duration1:0.000858991
duration2:8.711
total duration:8.71186
m_currentNum:33554433
bucketMem:134217728
nodeMem:805306392
total:896MiB
----------------------top--------------
root@localhost:/mnt/c/workspace# top -b |grep Cus
25189 zxiao     20   0  416148 152124   3512 S  16.0   1.9   0:00.48 CustomHashMap
25189 zxiao     20   0  809364 752688   3512 S 198.0   9.6   0:06.44 CustomHashMap
25189 zxiao     20   0 1202580   1.0g   3512 S 199.0  13.9   0:12.41 CustomHashMap
25189 zxiao     20   0 1321340   1.2g   3700 R 100.0  16.5   0:18.03 CustomHashMap
25189 zxiao     20   0 1206428   1.1g   3700 R 100.0  15.1   0:21.04 CustomHashMap
25189 zxiao     20   0 1202324   1.1g   3700 S   3.0  15.0   0:21.13 CustomHashMap
25189 zxiao     20   0 1202324   1.1g   3700 S   0.0  15.0   0:21.13 CustomHashMap
25189 zxiao     20   0 1202324   1.1g   3700 S   0.0  15.0   0:21.13 CustomHashMap
----------------------------------------------------------------------------------

zxiao@localhost:/mnt/c/workspace/hashmap/build$ ./CustomHashMap -me 2 -cb 24 -nb 25 -t 2 -p 64 -mu 32 -s 10
15
method=2
capBit=24
NodeBit=25
threadNum=2
partitionNum=64
mutexNum=32
starting to generate 2*16777216
data has been initilized, duration:0.480814
using:testCustomHashMap1WithMultiThreads
created CustomHashMap,capacity:16777216, total size:134217728
duration1:2.1919e-05
duration2:12.4382
total duration:12.4382
m_currentNum:33423851*24
bucketMem:134217728
nodeMem:802172424
total:893MiB
----------------------top-------------------
25718 zxiao     20   0  415764 138136   3192 S  16.0   1.8   0:00.48 CustomHashMap
25718 zxiao     20   0  677908 622392   3192 S 177.1   7.9   0:05.81 CustomHashMap
25718 zxiao     20   0  940052 888236   3192 S 176.7  11.3   0:11.11 CustomHashMap
25718 zxiao     20   0 1202196   1.1g   3192 S 173.4  14.0   0:16.33 CustomHashMap
25718 zxiao     20   0 1333268   1.2g   3192 S 171.3  16.4   0:21.47 CustomHashMap
25718 zxiao     20   0 1333268   1.2g   3316 R 100.0  16.7   0:24.62 CustomHashMap
25718 zxiao     20   0 1202192   1.1g   3404 S  21.7  15.0   0:25.27 CustomHashMap
25718 zxiao     20   0 1202192   1.1g   3404 S   0.0  15.0   0:25.27 CustomHashMap
----------------------------------------------------------------------------------

zxiao@localhost:/mnt/c/workspace/hashmap/build$ ./CustomHashMap -me 1 -cb 24 -nb 25 -t 2 -p 64 -mu 32 -s 10
15
method=1
capBit=24
NodeBit=25
threadNum=2
partitionNum=64
mutexNum=32
starting to generate 2*16777216
data has been initilized, duration:0.475575
using:testCustomHashMapWithMultiThreads
create CustomHashMap,capacity:16777216
duration1:0.246656
duration2:13.6896
total duration:13.9363
m_currentNum:33423810
bucketMem:402653184
nodeMem:534780960
total:894MiB
------------------top----------------------
25974 zxiao     20   0   71684  15124   3256 R   1.0   0.2   0:00.03 CustomHashMap
25974 zxiao     20   0  940052 912436   3508 S 157.3  11.6   0:04.75 CustomHashMap
25974 zxiao     20   0 1333268   1.3g   3508 S 172.8  16.7   0:09.95 CustomHashMap
25974 zxiao     20   0 1726484   1.6g   3508 S 173.7  20.7   0:15.16 CustomHashMap
25974 zxiao     20   0 1988628   1.8g   3508 S 172.4  24.2   0:20.35 CustomHashMap
25974 zxiao     20   0 2119700   2.0g   3692 R 100.0  26.7   0:24.97 CustomHashMap
25974 zxiao     20   0 2119700   2.0g   3692 R 100.0  26.7   0:27.97 CustomHashMap
25974 zxiao     20   0 1726480   1.6g   3692 S  80.1  21.7   0:30.38 CustomHashMap
25974 zxiao     20   0 1726480   1.6g   3692 S   0.0  21.7   0:30.38 CustomHashMap
25974 zxiao     20   0 1726480   1.6g   3692 S   0.0  21.7   0:30.38 CustomHashMap
25974 zxiao     20   0 1726480   1.6g   3692 S   0.0  21.7   0:30.38 CustomHashMap
-------------------------------------------------------------------------------------
zxiao@localhost:/mnt/c/workspace/hashmap/build$ ./CustomHashMap -me 0 -cb 24 -nb 25 -t 2 -p 64 -mu 32 -s 10
15
method=0
capBit=24
NodeBit=25
threadNum=2
partitionNum=64
mutexNum=32
starting to generate 2*16777216
data has been initilized, duration:0.469323
using:testUnorderedMap
duration1:1.099e-06
duration2:52.65
total duration:52.65
myHashMap size:56
Mem:33423560*8 not accurate!
------------------top----------------------
27786 zxiao     20   0  161812 110124   3408 S  96.7   1.4   0:02.91 CustomHashMap
27786 zxiao     20   0  161812 110124   3408 S   0.0   1.4   0:02.91 CustomHashMap
27837 zxiao     20   0  301568 184444   3228 S  33.9   2.3   0:01.02 CustomHashMap
27837 zxiao     20   0  373188 289880   3228 S 116.0   3.7   0:04.50 CustomHashMap
27837 zxiao     20   0  462384 412820   3228 S 115.3   5.3   0:07.97 CustomHashMap
27837 zxiao     20   0  641508 539620   3228 S 112.7   6.9   0:11.35 CustomHashMap
27837 zxiao     20   0  707044 652616   3228 S 117.6   8.3   0:14.89 CustomHashMap
27837 zxiao     20   0  838116 745280   3228 S 116.7   9.5   0:18.39 CustomHashMap
27837 zxiao     20   0 1030364 989316   3228 S 109.3  12.6   0:21.67 CustomHashMap
27837 zxiao     20   0 1066764 991792   3228 S 115.3  12.6   0:25.14 CustomHashMap
27837 zxiao     20   0 1197836   1.1g   3228 S 118.7  14.0   0:28.70 CustomHashMap
27837 zxiao     20   0 1328908   1.1g   3228 S 118.0  15.3   0:32.24 CustomHashMap
27837 zxiao     20   0 1328908   1.2g   3228 S 117.6  16.5   0:35.78 CustomHashMap
27837 zxiao     20   0 1459980   1.3g   3228 S 117.7  17.7   0:39.31 CustomHashMap
27837 zxiao     20   0 1981444   1.5g   3228 S 117.3  20.1   0:42.84 CustomHashMap
27837 zxiao     20   0 1981444   1.8g   3228 S  99.7  23.8   0:45.83 CustomHashMap
27837 zxiao     20   0 1789196   1.7g   3228 S 114.0  22.4   0:49.25 CustomHashMap
27837 zxiao     20   0 1920268   1.8g   3228 S 117.3  23.6   0:52.78 CustomHashMap
27837 zxiao     20   0 2051340   1.9g   3228 S 117.0  24.8   0:56.29 CustomHashMap
27837 zxiao     20   0 2116876   1.9g   3228 S 116.9  25.9   0:59.81 CustomHashMap
27837 zxiao     20   0 2116876   2.0g   3412 R 100.0  26.6   1:02.83 CustomHashMap
27837 zxiao     20   0 2116876   2.0g   3412 R 100.0  26.6   1:05.83 CustomHashMap
-------------------------------------------------------------------------------------