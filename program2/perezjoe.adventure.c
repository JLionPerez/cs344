#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include <stdbool.h>

/*struct room {
	char *name;
	char *room_type;
	int num_connections;
	int room_id;
	int room_connections[7];
};*/

int main() {
    DIR *dir;
    struct dirent *ent;

    /*function to get current directory (because they have different directories in server)*/

    if((dir = opendir (""))
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