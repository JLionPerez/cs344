#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

int main() {

	FILE * fp;
	int pid = getpid(); /*process ID of rooms*/
	int i = 0;
	char dir_name[300];	/*directory to rooms*/
	char * 

	sprintf(dir_name, "perezjoe.rooms.%d", pid); /*makes name of directory; change to include room*/
	mkdir(dir_name, 0777);	/*makes directory with all permissions*/

	/*printf("...\n");
	printf("Directory made.\n");*/

	/*fp = fopen (, "w");*/

	return 0;
}