#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	// only one process
	if (argc == 2)
	{
		execlp(argv[1], argv[1], NULL);
	}

	// two processes
	if (argc == 3)
	{
		int fds[2]; // fd[0] -> read end, fd[1] -> write end
		pipe(fds);
		pid_t p = fork();

		if (p < 0)
		{
			perror("fork fail");
			exit(1);
		}
		else if (p == 0)
		{
			// point stdout (fd 1) to write end of pipe (fd[1])
			printf("hello from first child!\n");
			dup2(fds[1], 1);
			close(fds[1]);

			// execute first command
			execlp(argv[1], argv[1], NULL);
		}
		else
		{
			int cpid = wait(NULL);
			printf("child %d exited\n", cpid);

			// create new child
			pid_t p2 = fork();

			if (p2 < 0)
			{
				perror("fork failed");
				exit(1);
			}
			else if (p2 == 0)
			{

				// point stdin to read end of pipe
				close(fds[1]); // close write end of pipe
				dup2(fds[0], 0);
				close(fds[0]);
				execlp(argv[2], argv[2], NULL);
			}
			else
			{
				int cpid2 = wait(NULL);
				printf("child %d exited\n", cpid2);
			}
		}
	}

	// three arguments
	else
	{
		// printf("hello from parent!\n");
		// int fds2[2];
		// pipe(fds2);
		// pid_t p2 = fork();
		// if (p2 < 0)
		// {
		// 	perror("fork fail");
		// 	exit(1);
		// }
		// if (p2 == 0)
		// {
		// 	printf("hello from second child!\n");
		// 	// point stdin to read end of pipe
		// 	dup2(fds[0], 0);
		// 	close(fds[0]);
		// 	// point stdout to write end of second pipe
		// 	dup2(fds2[1], 1);
		// 	close(fds2[1]);
		// 	// execute second command
		// 	execlp(argv[2], argv[2], NULL);
		// }
		// else
		// {
		// 	printf("hello from parent again!\n");
		// 	dup2(fds2[0], 0);
		// }
	}
}
