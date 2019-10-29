#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

/*struct room {
	char *name;
	char *room_type;
	int num_connections;
	int room_id;
	int room_connections[7];
};*/

int main() {
    char my_prefix[20] = "perezjoe.rooms.";
    char room_dir[300];
    DIR *current_dir = opendir(".");
    struct dirent *entry;
    struct stat entry_info;
    time_t recent_dir_time; /*st_mtime returns time/date which is also time_t*/

    /*function to get current directory (because they have different directories in server)*/

    if (current_dir > 0) {
        while((entry = readdir(current_dir)) != NULL) {
            /*printf("%s\n", entry->d_name);*/
            if(strstr(entry->d_name, my_prefix) != NULL) {
                stat(entry->d_name, &entry_info);
                printf("Found directories with timestamps: %s %s", entry->d_name, ctime(&entry_info.st_mtime));
                recent_dir_time = ctime(&entry_info.st_mtime);
            }
        }
        closedir(current_dir);
    }

    /*if((current_dir = opendir(".")) != NULL) {
        while((entry = readdir (current_dir) != NULL)) {
            printf("%s\n", entry->d_name);
        }
        closedir(current_dir);
    }

    else {
        perror("");
        return EXIT_FAILURE;
    }*/
    /*char *dir_path = "perezjoe.rooms. *";
    struct stat buf;
    stat(dir_path, &buf);

    int i;
    for(i = 0; i < 2; i++) {
        printf("File name: %s\n", dir_path);
        printf("Latest modified time: %s", ctime(&buf.st_mtime));
    }

    return 0;*/
}