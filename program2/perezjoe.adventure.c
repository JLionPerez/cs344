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
	int room_connections[7];
};

void find_dir(char *dir);

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

    FILE *file;

    while ((file_p = readdir(dir)) != NULL) {

        if((strcmp(file_p->d_name, "..") != 0) && (strcmp(file_p->d_name, ".") != 0)) {
            printf("%s\n", file_p->d_name);

        }
    }

    closedir(dir);

    return 0;
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