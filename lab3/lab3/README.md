# Hash Hash Hash

In this lab, we explore two implementations of hash tables optimized for multithreaded environments. The first version (`hash_table_v1`) employs a global mutex for thread safety, but at the cost of performance. The second version (`hash_table_v2`) builds upon v1 and introduces fine-grained locking to improve efficiency and scalability.

## Building

```shell
make
```

## Running

```shell
./hash-table-tester -t <number of threads> -s <number of entries>
```

## First Implementation

In the `hash_table_v1_add_entry` function of the hash table implementation, I introduced a mutex `entry_mutex` to synchronize access to the hash table. This was done to ensure thread safety when multiple threads attempt to add entries to the hash table simultaneously.

The use of a mutex prevents data races and ensures the consistency of the hash table's state. When a thread enters the `hash_table_v1_add_entry` function, it acquires the mutex lock. This prevents other threads from entering a critical section of the code until the mutex is unlocked. Specifically, this synchronization is important when checking for the existence of an entry and when inserting a new entry into the hash table.

### Performance

```shell
./hash-table-tester -t 10 -s 10000
Generation: 130,411 usec
Hash table base: 365,704 usec
    - 0 missing
Hash table v1: 490,264 usec
    - 0 missing
...
```

The mutex, while ensuring thread safety, introduces significant overhead. Since the mutex is global, it disallows parallelism in adding entries. Each thread must wait for the mutex to be released before it can perform an operation, leading to a performance bottleneck.

Further, v1 is slower than the base version because of the overhead of creating and destroying the threads and context switching between different threads. Since we actually don't get any parallelism, this is just extra work on top of the ultimately sequential hash table insertion in the v1 version, while the v0 version does not have such overhead.

## Second Implementation

In the `hash_table_v2_add_entry` function, I added fine-grained locking for each hash table entry. This version reduces lock contention and allows multiple threads to operate on different parts of the hash table concurrently.

### Performance

```shell
./hash-table-tester -t 4 -s 100000
Generation: 231,525 usec
Hash table base: 5,868,284 usec
    - 0 missing
    ...
Hash table v2: 1,404,754 usec
    - 0 missing
```

To measure the average performance increase, I did three trials of the command above (4 threads with 100,000 items):

| Trial   | Base Time | v2 Time |
| ------- | --------- | ------- |
| Trial 1 | 5.36s     | 1.43s   |
| Trial 2 | 6.07s     | 1.62s   |
| Trial 3 | 3.22s     | 1.25s   |
| Average | 4.88s     | 1.43s   |

The average speedup for v2 compared to the base version is `3.4` times faster.

There are significant performance benefits with the v2 version for hash tables holding very large numbers of elements. With a larger number of elements, the overhead of managing the locks is amortized, so v2 becomes faster almost by a factor of the number of cores.

## Cleaning up

```shell
make clean
```
