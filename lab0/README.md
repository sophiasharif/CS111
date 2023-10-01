# A Kernel Seedling

In this lab, we created a /proc/count file that shows the number of processes currently running with the command cat /proc/count. Since process information is internal to the kernel, we wrote a kernel module that creates a virtual /proc/count file, iterates through all the processes running and counts them, and writes the final count to the /proc/count file so that cat can output it.

## Building

```shell
make
```

## Running

```shell
sudo insmod proc_count.ko
cat /proc/count
```

After running the commands above, I saw that there were 135 proccess running on my VM.

## Cleaning Up

```shell
make clean
sudo rmmod proc_count.ko
```

## Testing

```python
python -m unittest
```

This was the output I got:

```
Ran 3 tests in 5.257s

OK
```

Report which kernel release version you tested your module on
(hint: use `uname`, check for options with `man uname`).
It should match release numbers as seen on https://www.kernel.org/.

```shell
uname -r -s -v
```

My kernel version:
Linux 5.14.8-arch-1 #1 SMP PREEMPT Sun, 26 Sep 2021 19:36:15 +0000
