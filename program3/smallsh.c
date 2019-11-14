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
void rep_pid(char *input);

int main() {
    char *line;
    shell_loop(line); //get user input

    return 0;
}

void commands(char *input) {
    
}

void rep_pid(char *input) {
    char strA[300];
    char strB[300];
    bool flag = true;
    bool switch;
    char *dd;

    // if(dd != NULL) {
    //     flag = true;
    // } else {
    //     flag = false;
    // }
    char* line = input;
    int pid = getpid();


    while(flag) {
        dd = strstr(line, "$$");
        
        if(dd == NULL) {
            flag = false;
            break;
        }

        dd[0] = '%';
        dd[1] = 'd';

        sprintf(new_str, input, pid);
    }
    printf("%s\n", new_str);
}

void shell_loop(char *input) {
    //char **args;
    ssize_t bufsize = 0; //initial 0
    bool stat = true;

    do {
        printf(": ");
        getline(&input, &bufsize, stdin);
        strtok(input, "\n"); //removes enter

        rep_pid(input);

        // char *dd = strstr(input, "$$"); //gets pointer of where $$ is, ex. dd[0] = '%'dd[1] = 'd' would replace string with 
        // dd[0] = '%';
        // dd[1] = 'd';
        // sprintf(new_str, input, getpid());

        // size_t i = 0;
        // while(new_str[i] != '\0') {
        //     char *dds = strstr(new_str, "$$"); //gets pointer of where $$ is, ex. dd[0] = '%'dd[1] = 'd' would replace string with 
        //     dds[0] = '%';
        //     dds[1] = 'd';
        //     sprintf(input, new_str, getpid());

        //     i++;
        // }
        // printf("%s\n", new_str);

        stat = false; //temporary
    } while(stat); //when stat is false it breaks otherwise keeps going
}