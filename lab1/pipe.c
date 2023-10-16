#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void errorExit(const char *message)
{
	perror(message);
	exit(1);
}

// creates a pipe, executes command in a child process, returns fd for read end of pipe.
int executeFirstCommand(char *command)
{
	int fds[2]; // fd[0] -> read end, fd[1] -> write end
	if (pipe(fds) == -1)
		errorExit("pipe failed");
	pid_t p = fork();

	if (p < 0)
		errorExit("fork failed");
	else if (p == 0) // child executes first command
	{
		dup2(fds[1], 1); // point stdout (fd 1) to write end of pipe (fd[1])
		close(fds[1]);	 // close write end of pipe
		execlp(command, command, NULL);
		errorExit("execlp failed");
	}
	else
	{
		int status;
		wait(&status); // wait for child to finish executing command
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			exit(WEXITSTATUS(status));

		close(fds[1]); // close write end of pipe for EOF; now all write fds are closed.
		return fds[0]; // return read end of pipe
	}
	return -1;
}

void executeLastCommand(char *command, int readEndOfPipe)
{
	pid_t p2 = fork(); // create new child

	if (p2 < 0)
		errorExit("fork failed");
	else if (p2 == 0)
	{
		dup2(readEndOfPipe, 0); // point stdin to read end of pipe
		close(readEndOfPipe);
		execlp(command, command, NULL);
		errorExit("execlp failed");
	}
	else
	{
		close(readEndOfPipe);
		int status;
		wait(&status); // wait for child to finish executing command
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			exit(WEXITSTATUS(status));
	}
}

int executeCommand(char *command, int readEndOfPipe)
{
	int fds[2]; // create pipe
	if (pipe(fds) < 0)
		errorExit("pipe failed");

	pid_t p = fork();
	if (p < 0)
		errorExit("fork failed");
	else if (p == 0)
	{
		dup2(readEndOfPipe, 0); // point std to read end of old pipe
		dup2(fds[1], 1);		// point stdout to write end of new pipe
		close(fds[0]);			// close fds; we no longer need them in the child
		close(fds[1]);
		execlp(command, command, NULL);
		errorExit("execlp failed");
	}
	else
	{
		close(readEndOfPipe); // close read end of old pipe and write end of
		close(fds[1]);		  // new pipe in parent; we no longer need them.

		int status;
		wait(&status); // wait for child to finish executing command
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			exit(WEXITSTATUS(status));

		return fds[0]; // return read end of new pipe
	}
	return -1;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Provide at least one command.\n");
		exit(1);
	}

	// only one process
	if (argc == 2)
		execlp(argv[1], argv[1], NULL);

	// multiple processes
	int readEndOfPipe = executeFirstCommand(argv[1]);
	for (int i = 2; i < argc - 1; i += 1)
		readEndOfPipe = executeCommand(argv[i], readEndOfPipe);
	executeLastCommand(argv[argc - 1], readEndOfPipe);
}
