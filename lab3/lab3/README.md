# Hash Hash Hash

In this lab, we create two different implementations of hash tables. The first version uses a global mutex; it is thread safe but performance is worse than the unparellized version because the lock add overhead but does not allow for any operations to happen concurrently. Version 2 builds on the foundational concepts of `hash_table_v1` and introduces improved synchronization mechanisms to enhance efficiency and scalability in multithreaded environments.

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

Although v1 is thread-safe, it performs worse than the base version because the overhead of acquiring and releasing a mutex is signifiacnt relative to the speed of the operations being performed and we don't have any parallelism in adding entries because the `hash_table_v1_add_entry` function can never run simultaneously on multiple threads.

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

There are significant performance benefits with the v2 version for hash tables holding very large numbers of elements. With a larger number of elements, the overhead of managing the locks is amortized, so v2 becomes faster almost by a factor of the number of threads.

## Cleaning up

```shell
make clean
```
