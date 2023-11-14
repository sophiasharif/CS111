# Hash Hash Hash

TODO introduction

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
```

Although v1 is thread-safe, it performs worse than the base version because the overhead of acquiring and releasing a mutex is signifiacnt relative to the speed of the operations being performed and we don't have any parallelism in adding entries because the `hash_table_v1_add_entry` function can never run simultaneously on multiple threads.

## Second Implementation

In the `hash_table_v2_add_entry` function, I TODO

### Performance

```shell
TODO how to run and results
```

TODO more results, speedup measurement, and analysis on v2

## Cleaning up

```shell
make clean
```
