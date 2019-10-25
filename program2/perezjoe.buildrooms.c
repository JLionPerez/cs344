#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

void shuffle(char *arr[]);

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

	shuffle(room_names);

	/*prints names of randomly generated names*/
	for (i = 0; i < 10; i++) {
		printf ("%s\n", room_names[i]);
	}

	/*writes in each file*/
	for (i = 0; i < 7; i++) {
		sprintf(dir_path, "perezjoe.rooms.%d/%s", pid, room_names[i]); /*adds file name to dir path*/

		fp = fopen (dir_path, "w"); /*opens file*/
		fprintf (fp, "File %d", i); /*write into file*/
		fclose(fp); /*close file*/
	}

	return 0;
}

/*does not need a reference, passing in a pointer gets dereferenced so the original array was changed*/
void shuffle (char *arr[]) {
	int i, rand_num;
	char *temp;

	for (i = 0; i < 10; i++) {
		rand_num = (rand() % (10-i)) + i;

		temp = arr[i];
		arr[i] = arr[rand_num];
		arr[rand_num] = temp;
	}
}