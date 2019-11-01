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
	/*int room_id;*/
	char *room_connections[7]; /*grabs the names of the connections*/
};

void find_dir(char *dir);
void initialize(struct room *list);

int main() {
	struct room room_list[7];
	char rooms_dir[300]; /*gets the name of the directory for latest time*/
	memset(rooms_dir, '\0', sizeof(rooms_dir));

	find_dir(rooms_dir);
	printf("%s\n", rooms_dir); /*prints current directory*/

	printf("\n");

	/*reiterating through the files in current directory*/
	struct dirent *file_p;
	DIR *dir = opendir(rooms_dir);
	chdir(rooms_dir); /*change into new directory*/
	FILE *file;

	char room_name[100];
	char room_connection[100];
	char room_type[100];
	int i;

	/* read through directory's files*/
	while ((file_p = readdir(dir)) != NULL) {

		if((strcmp(file_p->d_name, "..") != 0) && (strcmp(file_p->d_name, ".") != 0)) {

			for(i = 0; i < 7; i++) {

				printf("File name: %s\n", file_p->d_name); /*prints file name*/
				file = fopen(file_p->d_name, "r"); /*opens file*/

					while(fscanf(file, "ROOM NAME: %s\n", room_name) == 1){ 
						printf("Room %s\n", room_name);

					}

					while(fscanf(file, "CONNECTION %*d: %s\n", room_connection) > 0) {
						printf("Connection %s\n", room_connection);
					}

					while(fscanf(file, "ROOM TYPE: %s\n", room_type) == 1) {
						printf("Type %s\n", room_type);
					}

				fclose(file); /*close file*/
			}
		}
		printf("\n");
	}

	closedir(dir);

	return 0;
}

void initialize(struct room *list) {
	int i, j;

	for (i = 0; i < 7; i++) {
		list[i].num_connections = 0; /*every room has a starting 0 connections*/

		for (j = 0; j < 7; j++) {
			list[i].room_connections[j] = '\0'; /*fill each connection as null terminator*/
		}
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
				printf("Found directories with timestamps: %s %s\n", entry->d_name, ctime(&entry_info.st_mtime));
				
				/*compares times*/
				if((int)entry_info.st_mtime > recent_dir_time) {
					recent_dir_time = (int)entry_info.st_mtime;
					display_time = entry_info.st_mtime;
					memset(dir, '\0', sizeof(dir));
					strcpy(dir, entry->d_name);
					printf("Newer subdir: %s, new time: %s\n", entry->d_name, ctime(&entry_info.st_mtime));
				}
			}
		}

		closedir(current_dir);
	}
}