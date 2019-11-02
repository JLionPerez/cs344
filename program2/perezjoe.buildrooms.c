/*
* Title: Program 2 - Adventure
* Description: Two files, one generating random rooms within a direcotry and the other playing an adventure game using the rooms.
* Author: Joelle Perez
* Date: 1 Nov 2019
*/

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

/*connection functions*/
bool graphFull(struct room *list);
void addRandConnect(struct room *list);
struct room *getRandRoom(struct room *list);
bool canAddConnect(struct room *c);
bool connectionExists(struct room *a, struct room *b);
void connectRoom(struct room *a, struct room *b);
bool sameRoom(struct room *a, struct room *b);

int main() {
	srand(time(NULL));
	char *room_names[] = {"Bentley", "Lacey", "Kookie", "Coco", "Egg", "Mango", "Joe", "Elle", "Tabs", "Alien"};
	struct room room_list[7]; /*actual rooms*/

	shuffle(room_names); /*shuffles list of names*/
	form_structs(room_names, room_list); /*function to assign each name and room type to a struct room: id, name, room type*/
	initialize(room_list); /*set rooms to default elements, num_connections, room_connections*/

	/*adding connections*/
	while (graphFull(room_list) == false) {
		addRandConnect(room_list);
	}

	make_dir_files(room_names, room_list); /*makes files in directory*/

	return 0;
}

/*
* Function name: addRandConnect()
* Purpose: Adds random connections to each room
* Arguments: struct room *list
* Returns: none
* Based on pseudocode from 2.2 Outlining in Program 2
*/
void addRandConnect(struct room *list) {
	struct room *a;
	struct room *b;

	/*proceed when connections can be made otherwise get new room*/
	while(true) {
		a = getRandRoom(list);
		if(canAddConnect(a) == true) {
			break;
		}
	}

	/*make sure the second room can be used*/
	do {
		b = getRandRoom(list);
	} while(canAddConnect(b) == false || sameRoom(a, b) == true || connectionExists(a, b) == true);
	
	connectRoom(a, b);
}

/*
* Function name: printf_info()
* Purpose: Prints room infor for testing
* Arguments: struct room *list
* Returns: none
*/
void print_info(struct room *list) {
	int i, j;
	int count = 1;

	for(i = 0; i < 7; i++) {
		printf("ID %d\nRoom name: %s\nRoom type: %s\n# Connections: %d\n", list[i].room_id, list[i].name, list[i].room_type, list[i].num_connections);
		printf("ROOM CONNECTIONS\n");

		for(j = 0; j < 7; j++) {
			if (list[i].room_connections[j] == 1) {
				printf("Connection %d: %s\n", count, list[j].name);
				count++;
			}
		}
		count = 1;
		printf("\n");
	}
}

/*
* Function name: initialize()
* Purpose: Initializes the number of connections and indexes of the connections for each room
* Arguments: struct room *list
* Returns: none
*/
void initialize(struct room *list) {
	int i, j;

	for (i = 0; i < 7; i++) {
		list[i].num_connections = 0; /*every room has a starting 0 connections*/

		for (j = 0; j < 7; j++) {
			list[i].room_connections[j] = 0; /*fill each connection as false*/
		}
	}
}

/*
* Function name: graphFull()
* Purpose: Determine if all structs were filled with connections
* Arguments: struct room *list
* Returns: boolean
*/
bool graphFull(struct room *list) {
	int i;

	for (i = 0; i < 7; i++) {
		if(list[i].num_connections < 3 || list[i].num_connections > 6) {
			return false;
		}
	}

	return true;
}

/*
* Function name: getRandRoom()
* Purpose: Get's a randome room from the list
* Arguments: struct room *list
* Returns: struct room *
*/
struct room *getRandRoom(struct room *list) {
	int rand_index = rand() % 7;
	return &list[rand_index]; /*returns the address*/
}

/*
* Function name: canAddConnection()
* Purpose: To see if more connections can be made
* Arguments: struct room *c
* Returns: boolean
*/
bool canAddConnect(struct room *c) {
	if(c->num_connections < 6) {
		return true;
	}

	return false;
}

/*
* Function name: connectionExists()
* Purpose: Determines if a connections between the rooms already exit or not
* Arguments: struct room *a, struct *b
* Returns: booleans
*/
bool connectionExists(struct room *a, struct room *b) {
	/*if they are true at the index/id of the room_connections for both rooms, then they are connected*/
	if ((a->room_connections[b->room_id] == 1) && (b->room_connections[a->room_id] == 1)) { 
		return true;
	}

	return false;
}

/*
* Function name: connectRoom()
* Purpose: Connect the structs together by making each struct their connecting room
* Arguments: struct room *a, struct room *b
* Returns:  none
*/
void connectRoom(struct room *a, struct room *b) {
	a->room_connections[b->room_id] = 1; /*say true for index*/
	b->room_connections[a->room_id] = 1;

	a->num_connections++; /*increment # connections*/
	b->num_connections++;
}

/*
* Function name: sameRoom()
* Purpose: Determines whether two structs are the same
* Arguments: struct room *a, struct room *b
* Returns: boolean
*/
bool sameRoom(struct room *a, struct room *b) {
	/*if the ids for both structs are the same then true*/
	if(a->room_id == b->room_id) {
		return true;
	}

	return false;
}

/*
* Function name: form_structs()
* Purpose: Form the structs filling in the elements for each room
* Arguments: char *arr[], struct room *list
* Returns: none
*/
void form_structs(char *arr[], struct room *list) {
	int i;
	for (i = 0; i < 7; i++) {
		list[i].name = arr[i];
		list[i].room_id = i;

		/*first room is starting room*/
		if(i == 0) {
			list[i].room_type = "START_ROOM";
		}

		/*second room is end room*/
		else if (i == 1) {
			list[i].room_type = "END_ROOM";
		}

		else {
			list[i].room_type = "MID_ROOM";
		}
	}
}

/*
* Function name: make_dir_files()
* Purpose: To make a directory and the files within that directory
* Arguments: char *arr[], struct room *list
* Returns: none
*/
void make_dir_files(char *arr[], struct room *list) {
	FILE * fp;
	int i, j;
	int pid = getpid(); /*process ID of rooms*/
	char dir_name[300];	/*directory to rooms*/
	char dir_path[300];
	int countthis = 1;
	sprintf(dir_name, "perezjoe.rooms.%d", pid); /*makes name of directory; change to include room*/
	mkdir(dir_name, 0777);	/*makes directory with all permissions*/
	
	/*creates each file*/
	for (i = 0; i < 7; i++) {
		sprintf(dir_path, "perezjoe.rooms.%d/%s", pid, arr[i]); /*adds file name to dir path*/

		fp = fopen (dir_path, "w"); /*opens file*/
		fprintf(fp, "ROOM NAME: %s\n",list[i].name);

		for (j = 0; j < 7; j++) {
			if (list[i].room_connections[j] == 1) {
				fprintf(fp, "CONNECTION %d: %s\n", countthis, list[j].name);
				countthis++;
			}
		}
		countthis = 1;
		fprintf(fp, "ROOM TYPE: %s", list[i].room_type);
		fclose(fp); /*close file*/
	}
}

/*
* Function name: shuffle()
* Purpose: Shuffles the strings within an array
* Arguments: char *arr[]
* Returns: none
*/
void shuffle (char *arr[]) {
	int i, rand_num;
	char *temp;

	for (i = 0; i < 10; i++) {
		rand_num = (rand() % (10-i)) + i; /*randomly assign a number using the index*/

		/*switch names around*/
		temp = arr[i];
		arr[i] = arr[rand_num];
		arr[rand_num] = temp;
	}
}
