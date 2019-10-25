#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

void make_dir_files(char *arr[]);
void shuffle(char *arr[]);

int main() {
	srand(time(NULL));

	char *room_names[] = {"Bentley", "Lacey", "Kookie", "Coco", "Eggplant", "Mango", "Joe", "Elle", "Tabs", "Alien"};

	shuffle(room_names);

	/*temporary, prints names of randomly generated names*/
	int i;
	for (i = 0; i < 10; i++) {
		printf ("%s\n", room_names[i]);
	}

	make_dir_files(room_names);

	return 0;
}

void make_dir_files(char *arr[]) {
	FILE * fp;
	int i;
	int pid = getpid(); /*process ID of rooms*/
	char dir_name[300];	/*directory to rooms*/
	char dir_path[300];

	sprintf(dir_name, "perezjoe.rooms.%d", pid); /*makes name of directory; change to include room*/
	mkdir(dir_name, 0777);	/*makes directory with all permissions*/

	/*creates each file*/
	for (i = 0; i < 7; i++) {
		sprintf(dir_path, "perezjoe.rooms.%d/%s", pid, arr[i]); /*adds file name to dir path*/

		fp = fopen (dir_path, "w"); /*opens file*/
		fclose(fp); /*close file*/
	}
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
