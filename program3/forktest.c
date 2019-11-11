// fork() test from lecture 3.1 Processses

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void main() {

	pid_t spawnpid = -5; //initial initialization
	int ten = 10;

	spawnpid = fork(); //has a new value
	switch(spawnpid)
	{
		case -1: //if something went wrong, fork() returns -1 to the parent process and sets the global variable errno; no child process was created
			perror("Hull Breach!");
			exit(1);
			break;

		case 0: //in the child process, fork() returns 0
			ten = ten + 1;
			printf("I am the child! ten = %d\n", ten);
			break;

		default: //in the parent process, fork() returns the process id of the child process that was just created
			ten = ten - 1;
			printf("I am the parent! ten = %d\n", ten);
	}

	printf("This will be executed by both of us!\n");
	return;
}