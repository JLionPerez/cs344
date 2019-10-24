#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

int main() {

	// printf("Hello, Joelle. I've been birthed.\n");

	int pid = getpid(); //process ID of rooms
	char dir_name[300];	//director to rooms

	sprintf(dir_name, "perezjoe.rooms.%d", pid); //makes name of directory
	mkdir(dir_name, 0777);	//makes directory with all permissions

	printf("...\n");
	printf("Directory made.\n");

	return 0;
}