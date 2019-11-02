/*
* Title: Program 2 - Adventure
* Description: Plays the adventure game using the randomly generated files
* Author: Joelle Perez
* Date: 1 Nov 2019
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

struct room {
	char *name;
	char *room_type;
	int num_connections;
	int room_id; /*used to move through room*/
	char room_connections[7][100]; /*grabs the names of the connections*/
};

void find_dir(char *dir);
void initialize(struct room *list);
void print_info(struct room *list);
void read_files(char *new_dir, struct room *list);

void set_room(struct room *a, struct room *b);
void game();

int main() {
	struct room room_list[7];
	initialize(room_list);

	char rooms_dir[300]; /*gets the name of the directory for latest time*/
	memset(rooms_dir, '\0', sizeof(rooms_dir));

	find_dir(rooms_dir);
	read_files(rooms_dir, room_list);

	/*GAME unfinished*/
	struct room *current_room;
	struct room *start_room;
	struct room *end_room;
	bool end_game = false;
	char *input;
	size_t input_size = 32;
	size_t num_chars;
	int i, j, k;

	/*find start and end rooms*/
	for(i = 0; i < 7; i++) {
		if(strcmp(room_list[i].room_type, "START_ROOM") == 0) {
			start_room = &room_list[i];
			printf("Start room: %s\n", start_room->name);
		}

		else if (strcmp(room_list[i].room_type, "END_ROOM") == 0) {
			end_room = &room_list[i];
			printf("End room: %s\n", end_room->name);
		}
	}

	/*main game loop*/
	while(end_game == false) {
		printf("game start\n");

		current_room = start_room;
		printf("CURRENT LOCATION: %s\n", current_room->name);

		/*list connections for current room*/
		printf("POSSIBLE CONNECTIONS: ");
		for(j = 0; j < current_room->num_connections; j++) {
			if(j == current_room->num_connections-1) {
				printf("%s.\n", current_room->room_connections[j]);
			}

			else {
				printf("%s, ", current_room->room_connections[j]);
			}
		}

		/*ask user for input*/
		printf("WHERE TO? > ");
		getline(&input, &input_size, stdin);
		input[strlen(input)-1] = 0; /*gets rid of enter*/
		printf("\n");

		if(input == "time") {
			printf("not available\n");
		}
		else {
			int h;
			for(h = 0; h < current_room->num_connections; h++) {
				if(strcmp(input, current_room->room_connections[h])==0) {
					set_room(current_room, &room_list[h]);
					break;
				}
			}
		}
		/*Check if the input is valid*/
		/*for (k = 0; k < current_room->num_connections; k++) {
			printf("Input: %s\n", input);
			printf("current room: %s\n", current_room->room_connections[k]);
			if(strcmp(input, current_room->room_connections[k])!=0) {
				printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
				
			}

			else {
				int a;
				for(a = 0; a < 7; a++) {
					if(strcmp(input, room_list[a].name) == 0) {
						/*current_room = &room_list[a];*/
						/*set_room(current_room, &room_list[a]);
					}
				}
				
			}
		}*/

		/*printf("Inputted %s", input);*/
		/*end_game = true;*/
	}

	return 0;
}

/*
* Function name: set_room()
* Purpose: Sets the rooms to new room
* Arguments: struct room *a, struct room *b
* Returns: none
*/
void set_room(struct room *a, struct room *b) {
	int i;

	a->name = b->name;
	a->num_connections = b->num_connections;
	a->room_id = b->room_id;
	a->room_type = b->room_type;

	for (i = 0; i < b->num_connections; i++) {
		strcpy(a->room_connections[i], b->room_connections[i]);
	}

	a = b;
}

/*
* Function name: initialize()
* Purpose: Initialize each room's elements
* Arguments: struct room *list
* Returns: none
*/
void initialize(struct room *list) {
	int i, j;

	for (i = 0; i < 7; i++) {
		list[i].room_id = 0;
		list[i].num_connections = 0; /*every room has a starting 0 connections*/
		list[i].name = (char*)malloc(8*sizeof(char));
		list[i].room_type = (char*)malloc(10*sizeof(char));

		/*set each room's connections to null terminator*/
		for (j = 0; j < 7; j++) {
			memset(list[i].room_connections[j], '\0', sizeof(list[i].room_connections[j]));
		}
	}
}

/*
* Function name: printf_info()
* Purpose: Prints information of each room
* Arguments: struct *list
* Returns: none
*/
void print_info(struct room *list) {
	int i, j;
	for (i = 0; i < 7; i++) {
		printf("Name: %s\n", list[i].name);
		printf("ID# %d\n", list[i].room_id);
		printf("Num connections: %d\n", list[i].num_connections);

		/*print each connected rooms*/
		for(j = 0; j < list[i].num_connections; j++) {
			printf("Connections %d: %s\n", j+1, list[i].room_connections[j]);
		}

		printf("Type: %s\n", list[i].room_type);

		printf("\n");
	}
}

/*
* Function name: find_dir()
* Purpose: Find's the 
* Arguments: Finds the recently made directory
* Returns: none
* Cite: 2.4 Manipulating Directories Lectures
*	url: https://oregonstate.instructure.com/courses/1738955/pages/2-dot-4-manipulating-directories
*/
void find_dir(char *dir) {
	char my_prefix[20] = "perezjoe.rooms.";
	DIR *current_dir = opendir(".");
	struct dirent *entry;
	struct stat entry_info;
	int recent_dir_time = -1; /*st_mtime returns time/date which is also time_t*/
	time_t display_time;

	if (current_dir > 0) {
		while((entry = readdir(current_dir)) != NULL) {
			if(strstr(entry->d_name, my_prefix) != NULL) {
				stat(entry->d_name, &entry_info);
				
				/*compares times*/
				if((int)entry_info.st_mtime > recent_dir_time) {
					recent_dir_time = (int)entry_info.st_mtime;
					display_time = entry_info.st_mtime;

					memset(dir, '\0', sizeof(dir));
					strcpy(dir, entry->d_name);
				}
			}
		}

		closedir(current_dir);
	}
}

/*
* Function name: read_files()
* Purpose: Reads through each file and gathers information
* Arguments: char *new_dir, struct room *list
* Returns: none
*/
void read_files(char *new_dir, struct room *list) {
	struct dirent *file_p;
	DIR *dir = opendir(new_dir);
	chdir(new_dir); /*change into new directory*/
	FILE *file;

	char room_name[100];
	char room_connection[100];
	char type[100];
	int i = 0;

	/* read through directory's files*/
	while ((file_p = readdir(dir)) != NULL) {
		if((strcmp(file_p->d_name, "..") != 0) && (strcmp(file_p->d_name, ".") != 0)) {
			file = fopen(file_p->d_name, "r"); /*opens file*/

				/*find names for rooms*/
				while(fscanf(file, "ROOM NAME: %s\n", room_name) == 1){ 
					strcpy(list[i].name, room_name);
				}

				/*find connections for each room*/
				while(fscanf(file, "CONNECTION %*d: %s\n", room_connection) > 0) {
					strcpy(list[i].room_connections[list[i].num_connections], room_connection);
					list[i].num_connections++;
				}

				/*find room types for each room*/
				while(fscanf(file, "ROOM TYPE: %s\n", type) == 1) {
					strcpy(list[i].room_type, type);
				}

			list[i].room_id = i; /*set ids using index for each file*/

			i++;
			fclose(file); /*close file*/
		}
	}

	closedir(dir);
}