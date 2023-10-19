## UID: 105956925

## Pipe Up

An immitation of the pipe (|) operator in shells. Users pass in one-word executable names as command line arguments. Each process executes as a child process, and the output of one command is passed as input to the next.

## Building

To create the `pipe.o` executable, run the following command:

```shell
make
```

or:

```shell
make pipe
```

## Running

Show an example run of your program, using at least two additional arguments, and what to expect

### Example 1: ls | cat | wc

```shell
./pipe ls cat wc
```

output:

```shell
    6       6      51
```

### Example 2: ls | sort

```shell
./pipe ls sort
```

output:

```shell
Makefile
README.md
pipe
pipe.c
pipe.o
test_lab1.py
```

## Cleaning up

To clean up binary files, run:

```shell
make clean
```
