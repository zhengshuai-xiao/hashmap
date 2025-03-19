# hashmap

This is used for testing hashmap to insert billion of data.
there are 4 hashmap implemented:
    1.std::unordered_map
    2.std::vector + std::list + shared_mutex pool
    3.array + list + shared_mutex pool
    4.partition + #3

I would like to show which has better performance on putting/getting data in parallel.

I got the following things:
1. One hashmap with multiple mutex(64) to protect the corresponding data has same performance as multiple hashmap(64) with one mutex.
    but I think method #4 is better than #3, because it is more flexible on hashmap extending
2. Method #3 and #4 has better performance on inserting data.
3. At least 2/4 threads will make #3 and #4's insert performance better. 
4. Method #3 & #4 use much less memory than #1 & #2, BTW, #1 and #2 use same memory when loading same data.

TODO:
hashmap extending(rehash)

-Usage:
-me method: 0:unordered_map;
    1: std::vector + std::list + shared_mutex pool;
    2: array + list + shared_mutex pool
    3: partition + array + list + shared_mutex pool
-t thread: uses how many threads to insert data
-cb capBit: 1~32
-nb NodeBit: 1~32
-p part: default:64
-mu mutex num: default:64
-s sleep default:100s
