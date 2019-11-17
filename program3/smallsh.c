#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define TOT_ARGS 512
#define TOT_CHARS 2048

//CURRENTLY WORKING ON: cd
//GOAL: finish cd
//NEXT GOAL: status

//prototypes
void shell_loop(char *input);
void rep_pid(char *input);
void parse(char *input, char **arguments, int *num_els);
void commands(char **arguments, int num_els);
void change_dir(char **aruments, int num_args);

int main() {
    char *c_line;
    shell_loop(c_line); //get user input

    return 0;
}

void shell_loop(char *input) {
    ssize_t bufsize = 0; //initial 0
    bool stat = true;
    char **args = malloc(TOT_ARGS * sizeof(char *));
    int num_line_elements = 0;

    do {
        printf(": ");
        getline(&input, &bufsize, stdin);
        strtok(input, "\n"); //removes enter

        if (strlen(input) > TOT_CHARS) {
            printf("error too many characters\n");
        }

        else {
            rep_pid(input); //$$ to pid
            parse(input, args, &num_line_elements); //parse in arguments
            commands(args, num_line_elements); //finds built in commands
        }

        fflush(stdout); //clears stdout buffer 

        //stat = false; //when exiting turn stat to false
    } while(stat); //when stat is false it breaks otherwise keeps going
}

void commands(char **arguments, int num_els) { //3 built-in commands
    int command_args = num_els - 1; //not include command itself

    if(strcmp(arguments[0], "cd") == 0) {
        printf("changing directories\n");
        change_dir(arguments, command_args);
    }

    else if(strcmp(arguments[0], "status") == 0) {
        printf("showing status\n");
    }

    else if(strcmp(arguments[0], "exit") == 0) {
        printf("exiting now\n");
    }
}

void change_dir(char **arguments, int num_args) {
    char path_dir[256];
    char new_cwd[256];
    char *cur_dir;

    //printf("Num args: %d\n", num_args);

    if(num_args == 0) { //go to home dir when no args after command
        //printf("in if\n");
        //path_dir = getenv("HOME");
        strcpy(path_dir, getenv("HOME"));
        chdir(path_dir);
        //printf("Path: %s\n", path_dir);
    }

    else if(num_args == 1) { //for absolute and relative paths
        strcpy(path_dir, arguments[1]);
        chdir(path_dir);
    }

    else {
        printf("error too many arguments for cd\n");
    }

    cur_dir = getcwd(new_cwd, sizeof(new_cwd));
    //printf("Current dir: %s\n", cur_dir);
}

void parse(char *input, char **arguments, int *num_els) {
        int i = 0;
        int elements = 0;
        char *arg = strtok(input, " ");

        while(arg != NULL) {
            if(i > TOT_ARGS) {
                printf("error too many arguments");
                break;
            }

            arguments[i] = arg;
            i++;
            elements++;

            arg = strtok(NULL, " ");
        }

        *num_els = elements; //dereference passed arg to change to actual elements
}

void rep_pid(char *input) { //changes $$ to pid
    char new_str[300];
    char *dd = strstr(input, "$$");

    if(dd != NULL) {
        dd[0] = '%';
        dd[1] = 'd';
        sprintf(new_str, input, getpid());
        strcpy(input, new_str);
        printf("%s\n", input);
    }
}