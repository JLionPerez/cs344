#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

struct room {
	char *name;
	char *room_type;
	int num_connections;
	struct room *room_connections[6];
};

void make_dir_files(char *arr[], struct room *list);
/*void write_into_file(FILE *file, struct room *list);*/
void shuffle(char *arr[]);
void form_structs(char *arr[], struct room *list);

int main() {
	srand(time(NULL));
	char *room_names[] = {"Bentley", "Lacey", "Kookie", "Coco", "Eggplant", "Mango", "Joe", "Elle", "Tabs", "Alien"};
	struct room *room_list = (struct room *)malloc(sizeof(struct room) * 7);

	shuffle(room_names); /*shuffles list of names*/
	form_structs(room_names, room_list); /*function to assign each name and room type to a struct room*/
	/*maybe? function to assign connecting room of each struct rooms*/
	make_dir_files(room_names, room_list); /*makes files in directory*/

	/*temporary, prints rooms' name and type*/
	int i;
	for (i = 0; i < 7; i++) {
		printf("Room %d is %s and type is %s.\n", i, room_list[i].name, room_list[i].room_type);
	}
	printf("\n");

	return 0;
}

/*assigns names and room type*/
void form_structs(char *arr[], struct room * list) {
	int i;
	for (i = 0; i < 7; i++) {
		list[i].name = arr[i];

		if(i == 0) { /*first room is starting room*/
			list[i].room_type = "START_ROOM";
		}

		else if (i == 1) { /*second room is end room*/
			list[i].room_type = "END_ROOM";
		}

		else {
			list[i].room_type = "MID_ROOM";
		}

		/*printf("Room %d is %s and type is %s.\n", i, list[i].name, list[i].room_type);*/
	}
}

void make_dir_files(char *arr[], struct room *list) {
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

		/*function to write data into files*/
		fp = fopen (dir_path, "w"); /*opens file*/
		fprintf(fp, "ROOM NAME: %s\n",list[i].name);
		/*have a function to print out the connections for each room ex. write_connections(FILE *file, char path, struct room *list)*/
		fprintf(fp, "ROOM TYPE: %s", list[i].room_type);
		fclose(fp); /*close file*/
	}
}

/*void write_into_file(FILE *file, struct room *list) {
	file = fopen (dir_path, "w"); /*opens file*/
	/*fclose(file); /*close file*/
/*}*/

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
