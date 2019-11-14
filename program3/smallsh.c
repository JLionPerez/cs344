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
    //switch statements for commands
    
}

void rep_pid(char *input) { //only gets $$ once or else inifite loop
    char new_str[300];
    char *dd = strstr(input, "$$");

    if(dd != NULL) {
        dd[0] = '%';
        dd[1] = 'd';
        sprintf(new_str, input, getpid());
        printf("%s\n", new_str);
    }
    // char strA[300];
    // char strB[300];
    // bool flag = true;
    // bool switched = true;
    // char *dd;
    
    // char* line = input;
    // printf("Line is input\n");
    // int pid = getpid();

    // while(flag) {

    //     dd = strstr(line, "$$");
        
    //     if(dd == NULL) {
    //         printf("Flag is false\n");
    //         flag = false;
    //         break;
    //     }

    //     printf("Flag is true\n");

    //     dd[0] = '%';
    //     dd[1] = 'd';

    //     if(switched) {
    //         printf("Line is A\n");
    //         line = strA;
    //         switched = false;
    //     } else {
    //         printf("Line is B\n");
    //         line = strB;
    //         switched = true;
    //     }

    //     sprintf(line, input, pid);
    //     printf("%s\n", line);
    // }
}

void shell_loop(char *input) {
    char **args; //holds arguments
    ssize_t bufsize = 0; //initial 0
    bool stat = true;
    int i = 0;

    do {
        printf(": ");
        getline(&input, &bufsize, stdin);
        strtok(input, "\n"); //removes enter

        if (strlen(input) > 2048) {
            printf("error too many characters\n");
        }

        while() { //goal: parse in arguments
            strtok(input, " ");
            //i++;
        }

        printf("%s\n", input);

        //rep_pid(input);
        fflush(stdout);

        //stat = false; //temporary
    } while(stat); //when stat is false it breaks otherwise keeps going
}