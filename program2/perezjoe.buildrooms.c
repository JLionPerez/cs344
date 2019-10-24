#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

int randomize();

int main() {
	srand(time(NULL));

	FILE * fp;
	int pid = getpid(); /*process ID of rooms*/
	char dir_name[300];	/*directory to rooms*/
	char dir_path[300];
	char *room_names[] = {"Bentley", "Lacey", "Kookie", "Coco", "Eggplant", "Mango", "Joe", "Elle", "Tabs", "Alien"};
	int i;

	sprintf(dir_name, "perezjoe.rooms.%d", pid); /*makes name of directory; change to include room*/
	mkdir(dir_name, 0777);	/*makes directory with all permissions*/

	randomize(room_names);

	/*for (i = 0; i < 7; i++) {
		sprintf(dir_path, "perezjoe.rooms.%d/%s", pid, room_names[i]); /*adds file name to dir path*/

		/*fp = fopen (dir_path, "w"); /*opens file*/
		/*fprintf (fp, "File %d", i); /*write into file*/
		/*fclose(fp); /*close file*/
	/*}*/

	return 0;
}

int randomize() {
	int i = rand() % 9, j = rand() % 9;

	if(i != j) {
		return i;
	}

	else {
		i = rand() % 9;
	}
 
	/*int i = rand() % 9, j = rand() % 9;
	char *temp = arr[i];

	arr[i] = arr[j];
	arr[j] = temp;

	printf("temp: %s\n", temp);
	printf("%s\n", arr[i]);
	printf("%s\n", arr[j]);*/
	/*printf("%d\n", i);*/
	/*printf("%d\n", j);*/
}