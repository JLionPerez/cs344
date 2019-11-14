#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// GOAL: Prompt
// NEXT GOAL: Built-In Commands

//prototypes
void shell_loop(char *input);
void commands(char *input);

int main() {
    char *line;
    shell_loop(line); //get user input

    return 0;
}

void commands(char *input) {

}

void shell_loop(char *input) {
    //char **args;
    char new_str[100];
    ssize_t bufsize = 0; //initial 0
    bool stat = true;

    do {

        // if (strcmp(line, "exit") == 0) {
        //     stat = false;
        // }

        printf(": ");
        getline(&input, &bufsize, stdin);
        strtok(input, "\n");

        char *dd = strstr(input, "$$"); //gets pointer of where $$ is, ex. dd[0] = '%'dd[1] = 'd' would replace string with 
        dd[0] = '%';
        dd[1] = 'd';
        sprintf(new_str, input, getpid());

        printf("%s", new_str);
        //sprintf(input, "%d", );

        // size_t i = 0;
        // while(input[i] != '\0') {

        //     //printf("%c\n", input[i]);
        //     i++;
        // }
        //printf("Repeated: %s", input);

        stat = false; //temporary
    } while(stat); //when stat is false it breaks otherwise keeps going
}