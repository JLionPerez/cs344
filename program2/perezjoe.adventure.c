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

void get_room_id(struct room *list);
void get_room(struct room *list);
void game();

int main() {
	struct room room_list[7];
	initialize(room_list);

	char rooms_dir[300]; /*gets the name of the directory for latest time*/
	memset(rooms_dir, '\0', sizeof(rooms_dir));

	find_dir(rooms_dir);
	read_files(rooms_dir, room_list);

	print_info(room_list);

	/*GAME*/
	struct room *current_room;
	bool end_game = false;
	int i;

	while(end_game == false) {
		/*look through each room*/
		printf("game start\n");
		for(i = 0; i < 7; i++) {
			if(room_list[i].room_type == "START_ROOM") {
				current_room = &room_list[i];

				printf("Current room: %s\n", current_room->name);
				end_game = true;
			}
		}
	}

	return 0;
}

void get_room_id(struct room *list) {

}

void get_room(struct room *list) {

}

void initialize(struct room *list) {
	int i, j;

	for (i = 0; i < 7; i++) {
		list[i].room_id = 0;
		list[i].num_connections = 0; /*every room has a starting 0 connections*/
		list[i].name = (char*)malloc(8*sizeof(char));
		list[i].room_type = (char*)malloc(10*sizeof(char));

		for (j = 0; j < 7; j++) {
			memset(list[i].room_connections[j], '\0', sizeof(list[i].room_connections[j]));
		}
	}
}

void print_info(struct room *list) {
	int i, j;
	for (i = 0; i < 7; i++) {
		printf("Name: %s\n", list[i].name);
		printf("ID# %d\n", list[i].room_id);
		printf("Num connections: %d\n", list[i].num_connections);

		for(j = 0; j < list[i].num_connections; j++) {
			printf("Connections %d: %s\n", j+1, list[i].room_connections[j]);
		}

		printf("Type: %s\n", list[i].room_type);

		printf("\n");
	}
}

/*cited from 2.4 Manipulating Directories Lectures https://oregonstate.instructure.com/courses/1738955/pages/2-dot-4-manipulating-directories*/
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
				/*printf("Found directories with timestamps: %s %s\n", entry->d_name, ctime(&entry_info.st_mtime));*/
				
				/*compares times*/
				if((int)entry_info.st_mtime > recent_dir_time) {
					recent_dir_time = (int)entry_info.st_mtime;
					display_time = entry_info.st_mtime;
					memset(dir, '\0', sizeof(dir));
					strcpy(dir, entry->d_name);
					/*printf("Newer subdir: %s, new time: %s\n", entry->d_name, ctime(&entry_info.st_mtime));*/
				}
			}
		}

		closedir(current_dir);
	}
}

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

			/*printf("File name: %s\n", file_p->d_name); /*prints file name*/
			file = fopen(file_p->d_name, "r"); /*opens file*/

				while(fscanf(file, "ROOM NAME: %s\n", room_name) == 1){ 
					/*printf("Room %s\n", room_name);*/
					strcpy(list[i].name, room_name);
				}

				while(fscanf(file, "CONNECTION %*d: %s\n", room_connection) > 0) {
					/*printf("Connection %s\n", room_connection);*/
					strcpy(list[i].room_connections[list[i].num_connections], room_connection);
					list[i].num_connections++;
				}

				/*printf("Number of connections for file %s is %d.\n", room_list[i].name, room_list[i].num_connections);*/

				while(fscanf(file, "ROOM TYPE: %s\n", type) == 1) {
					/*printf("Type %s\n", type);*/
					strcpy(list[i].room_type, type);
				}

			list[i].room_id = i;
			/*printf("ID %d\n", room_list[i].room_id);*/

			i++;
			fclose(file); /*close file*/
		}
	}

	closedir(dir);
}