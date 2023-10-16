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
		close(fds[1]);	 // close write end of pipe for EOF
		execlp(command, command, NULL);
		errorExit("execlp failed");
	}
	else
	{
		int cpid = wait(NULL); // wait for child to finish command
		close(fds[1]);		   // close write end of pipe for EOF; now all write fds are closed.
		return fds[0];		   // return read end of pipe
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
		int cpid2 = wait(NULL); // wait for process to finish
	}
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

	// two processes
	if (argc == 3)
	{
		int readEndOfFirstPipe = executeFirstCommand(argv[1]);
		executeLastCommand(argv[2], readEndOfFirstPipe);
	}
}
