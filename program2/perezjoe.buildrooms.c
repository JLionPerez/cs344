#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>

struct room {
	char *name;
	char *room_type;
	int num_connections;
	int room_id;
	int room_connections[7]; /*bools, 1 is true, 0 is false*/
};

void initialize(struct room *list);
void print_info(struct room *list);
void make_dir_files(char *arr[], struct room *list);
void shuffle(char *arr[]);
void form_structs(char *arr[], struct room *list);
void write_connections(FILE *file, char path, struct room *list);

/*connection functions*/
bool graphFull(struct room *list);
void addRandConnect(struct room *list);
struct room getRandRoom(struct room *list);
bool canAddConnect(struct room c);
bool connectionExists(struct room a, struct room c);
void connectRoom(struct room a, struct room b);
bool sameRoom(struct room a, struct room b);

int main() {
	srand(time(NULL));
	char *room_names[] = {"Bentley", "Lacey", "Kookie", "Coco", "Egg", "Mango", "Joe", "Elle", "Tabs", "Alien"};
	struct room room_list[7]; /*actual rooms*/

	shuffle(room_names); /*shuffles list of names*/
	form_structs(room_names, room_list); /*function to assign each name and room type to a struct room: id, name, room type*/
	initialize(room_list); /*set rooms to default elements, num_connections, room_connections*/

	/*while (graphFull(room_list) == false) { /*adding connections*/
		/*addRandConnect(room_list);
		break;
	}*/

	printf("Connections made. Starting files.\n");

	make_dir_files(room_names, room_list); /*makes files in directory*/

	/*temporary, prints rooms' name and type*/
	int i;
	for (i = 0; i < 7; i++) {
		printf("Room %d is %s and type is %s.\n", i, room_list[i].name, room_list[i].room_type);
	}
	printf("\n");

	return 0;
}

/*connections*/
void addRandConnect(struct room *list) {
	struct room a, b;

	while(true) {
		a = getRandRoom(list);

		if(canAddConnect(a) == true) {
			break;
		}
	}

	do {
		b = getRandRoom(list);
	} while(canAddConnect(b) == false || sameRoom(a, b) == true || connectionExists(a, b) == true);
	
	connectRoom(a, b);
	connectRoom(b, a);
}

void print_info(struct room *list) {
	int i;

	for(i = 0; i < 7; i++) {
		printf("ID %d\nRoom name: %s\n", list[i].room_id, list[i].name);
	}
}

void initialize(struct room *list) {
	int i;

	for (i = 0; i < 7; i++) {
		list[i].num_connections = 0; /*every room has a starting 0 connections*/
		list[i].room_connections[i] = 0; /*fill each connection as false*/
	}
}

bool graphFull(struct room *list) {
	int i;

	for (i = 0; i < 7; i++) {
		if(list[i].num_connections < 3) {
			return false;
		}
	}

	return true;
}

struct room getRandRoom(struct room *list) {

}

bool canAddConnect(struct room c) {

}

bool connectionExists(struct room a, struct room b) {

}

void connectRoom(struct room a, struct room b) {
	a.room_connections[b.room_id] = 1; /*say true for index*/
	b.room_connections[a.room_id] = 1;

	a.num_connections++;
	b.num_connections++;
}

bool sameRoom(struct room a, struct room b) {

}

/*assigns names and room type*/
void form_structs(char *arr[], struct room *list) {
	int i;
	for (i = 0; i < 7; i++) {
		list[i].name = arr[i];
		list[i].room_id = i;

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
