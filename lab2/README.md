# You Spin Me Round Robin

This implementation focuses on round robin scheduling for a given workload and quantum length. It utilizes the `TAILQ` from `sys/queue.h` for managing the processes. The input to the program is a file that lists processes with their respective arrival and burst times, and either a fixed quantum length or `median` as the quantum length specification.

## Building

To build the rr executable from the provided source code, navigate to the lab-02 directory and run the following command:

```shell
make
```

This command compiles the code and creates the rr executable within the same directory.

## Running

To execute the rr program, you need to provide two arguments: the path to your processes.txt file and the quantum length (which can be a positive integer or the string 'median'). For example, if you want to use a fixed quantum length of 30, you can run:

```shell
./rr processes.txt 30
```

Upon execution, the program will output the average waiting time and response time for the provided list of processes. Sample output might look something like this:

```shell
Average waiting time: 7.00
Average response time: 2.75
```

Note: The actual values might vary based on the input processes and quantum length.

## Cleaning up

If you wish to remove all binary files and executables created during the build process, simply run:

```shell
make clean
```

This command ensures your directory remains clean, containing only the necessary source files.
