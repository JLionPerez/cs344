#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// GOAL: Prompt
// NEXT GOAL: Built-In Commands

//prototypes
void shell_loop();

int main(int argc, char **argv) {

    shell_loop();

    return EXIT_SUCCESS;
}

void shell_loop() {
    char *line = NULL;
    char **args;
    ssize_t bufsize = 0;
    bool stat = true;

    do {
        printf(": ");
        //read line input
        getline(&line, &bufsize, stdin);
        //split line input
        //execute command

        printf("Repeated: %s", line);

        // free(line);
        // free(args);
        stat = false; 
    } while(stat); //when stat is false it breaks otherwise keeps going
}