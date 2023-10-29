#include <fcntl.h>
#include <stdbool.h>
#include <stdckdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <unistd.h>

/* A process table entry.  */
struct process
{
  long pid;
  long arrival_time;
  long burst_time;

  TAILQ_ENTRY(process)
  pointers;

  /* Additional fields here */
  int schedule_time;
  int idle_time;
  long running_time;
  /* End of "Additional fields here" */
};

TAILQ_HEAD(process_list, process);

/* Skip past initial nondigits in *DATA, then scan an unsigned decimal
   integer and return its value.  Do not scan past DATA_END.  Return
   the integer’s value.  Report an error and exit if no integer is
   found, or if the integer overflows.  */
static long
next_int(char const **data, char const *data_end)
{
  long current = 0;
  bool int_start = false;
  char const *d;

  for (d = *data; d < data_end; d++)
  {
    char c = *d;
    if ('0' <= c && c <= '9')
    {
      int_start = true;
      if (ckd_mul(&current, current, 10) || ckd_add(&current, current, c - '0'))
      {
        fprintf(stderr, "integer overflow\n");
        exit(1);
      }
    }
    else if (int_start)
      break;
  }

  if (!int_start)
  {
    fprintf(stderr, "missing integer\n");
    exit(1);
  }

  *data = d;
  return current;
}

/* Return the first unsigned decimal integer scanned from DATA.
   Report an error and exit if no integer is found, or if it overflows.  */
static long
next_int_from_c_str(char const *data)
{
  return next_int(&data, strchr(data, 0));
}

/* A vector of processes of length NPROCESSES; the vector consists of
   PROCESS[0], ..., PROCESS[NPROCESSES - 1].  */
struct process_set
{
  long nprocesses;
  struct process *process;
};

/* Return a vector of processes scanned from the file named FILENAME.
   Report an error and exit on failure.  */
static struct process_set
init_processes(char const *filename)
{
  int fd = open(filename, O_RDONLY);
  if (fd < 0)
  {
    perror("open");
    exit(1);
  }

  struct stat st;
  if (fstat(fd, &st) < 0)
  {
    perror("stat");
    exit(1);
  }

  size_t size;
  if (ckd_add(&size, st.st_size, 0))
  {
    fprintf(stderr, "%s: file size out of range\n", filename);
    exit(1);
  }

  char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED)
  {
    perror("mmap");
    exit(1);
  }

  char const *data_end = data_start + size;
  char const *data = data_start;

  long nprocesses = next_int(&data, data_end);
  if (nprocesses <= 0)
  {
    fprintf(stderr, "no processes\n");
    exit(1);
  }

  struct process *process = calloc(sizeof *process, nprocesses);
  if (!process)
  {
    perror("calloc");
    exit(1);
  }

  for (long i = 0; i < nprocesses; i++)
  {
    process[i].pid = next_int(&data, data_end);
    process[i].arrival_time = next_int(&data, data_end);
    process[i].burst_time = next_int(&data, data_end);
    if (process[i].burst_time == 0)
    {
      fprintf(stderr, "process %ld has zero burst time\n",
              process[i].pid);
      exit(1);
    }
  }

  if (munmap(data_start, size) < 0)
  {
    perror("munmap");
    exit(1);
  }
  if (close(fd) < 0)
  {
    perror("close");
    exit(1);
  }
  return (struct process_set){nprocesses, process};
}

void print_list(struct process_list list)
{
  struct process *curr;
  TAILQ_FOREACH(curr, &list, pointers)
  {
    printf("%ld [arr %ld, burst %ld] -> ", curr->pid, curr->arrival_time, curr->burst_time);
  }
  printf("\n");
}

int len(struct process_list list)
{
  int counter = 0;
  struct process *curr;
  TAILQ_FOREACH(curr, &list, pointers)
  counter++;
  return counter;
}

int cmpfunc(const void *a, const void *b)
{
  return (*(int *)a - *(int *)b);
}

int find_median(struct process_list *list, int runtimes[])
{
  // put times of known processes into runtimes array
  struct process *curr;
  int length = 0;
  TAILQ_FOREACH(curr, list, pointers)
  {
    runtimes[length] = curr->running_time;
    length++;
  }

  // sort runtimes
  qsort(runtimes, length, sizeof(int), cmpfunc);

  // get median
  int median;
  if (length == 0)
    median = 1;
  else if (length % 2 == 1)
    median = runtimes[length / 2];
  else
    median = (runtimes[length / 2 - 1] + runtimes[length / 2]) / 2;

  printf("runtimes: ");
  for (int i = 0; i < length; i++)
  {
    printf("%d ", runtimes[i]);
  }
  printf("|| median: %d \n", median);

  return median ? median : 1;
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    fprintf(stderr, "%s: usage: %s file quantum\n", argv[0], argv[0]);
    return 1;
  }

  struct process_set ps = init_processes(argv[1]);
  long quantum_length = (strcmp(argv[2], "median") == 0 ? -1
                                                        : next_int_from_c_str(argv[2]));
  if (quantum_length == 0)
  {
    fprintf(stderr, "%s: zero quantum length\n", argv[0]);
    return 1;
  }

  struct process_list list;
  TAILQ_INIT(&list);

  long total_wait_time = 0;
  long total_response_time = 0;

  /* Your code here */
  int clock = 0;
  int curr_quant = quantum_length;
  int num_completed = 0;
  bool running = false;
  bool context_switch = false;

  int runtimes[ps.nprocesses]; // passed into median calculator

  while (num_completed < ps.nprocesses)
  {
    if (context_switch)
    {
      running = false;
      context_switch = false;
      total_wait_time++;
      printf("time %d [CONTEXT SWITCH]\n", clock);
    }

    // check if processes have completed running
    if (running && !TAILQ_EMPTY(&list))
    {
      struct process *curr = TAILQ_FIRST(&list);
      curr->running_time++;
      curr_quant--;

      // process completed; remove from queue
      if (curr->running_time == curr->burst_time)
      {
        TAILQ_REMOVE(&list, curr, pointers);
        num_completed++;
        running = false;
        context_switch = true;
        printf("time %d [PROCESS %ld FINISH] (new length %d): ", clock, curr->pid, len(list));
        print_list(list);
      }

      // current quantum completed; move process to end of queue
      else if (curr_quant == 0)
      {
        TAILQ_REMOVE(&list, curr, pointers);
        TAILQ_INSERT_TAIL(&list, curr, pointers);
        running = false;

        if (curr->pid != TAILQ_FIRST(&list)->pid)
          context_switch = true;

        printf("time %d [QUANTUM END]: ", clock);
        print_list(list);
      }
    }

    // add incoming processes
    for (int i = 0; i < ps.nprocesses; i++)
    {
      if (ps.process[i].arrival_time == clock)
      {
        struct process *curr = ps.process + i;
        curr->idle_time = 0;
        curr->schedule_time = -1;
        curr->running_time = 0;
        TAILQ_INSERT_TAIL(&list, curr, pointers);
        printf("time %d [NEW PROCESS ARRIVAL]: added process %ld to queue\n", clock, curr->pid);
      }
    }

    // schedule next process
    if (!running & !TAILQ_EMPTY(&list))
    {
      // reset current quantum
      curr_quant = quantum_length == -1 ? find_median(&list, runtimes) : quantum_length;
      printf("time %d ", clock);
      find_median(&list, runtimes);

      // "schedule" the next process
      struct process *curr = TAILQ_FIRST(&list);
      if (curr->schedule_time == -1)
      {
        curr->schedule_time = clock + 1;
        total_response_time += clock - curr->arrival_time;
        total_response_time += context_switch;
      }

      running = true;
    }

    clock++;
    total_wait_time += len(list) ? len(list) - 1 : 0;
  }

  print_list(list);

  /* End of "Your code here" */

  printf("Average wait time: %.2f\n",
         total_wait_time / (double)ps.nprocesses);
  printf("Average response time: %.2f\n",
         total_response_time / (double)ps.nprocesses);

  if (fflush(stdout) < 0 || ferror(stdout))
  {
    perror("stdout");
    return 1;
  }

  free(ps.process);
  return 0;
}
