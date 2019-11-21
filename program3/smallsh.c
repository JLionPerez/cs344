#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>

#define TOT_ARGS 512
#define TOT_CHARS 2048

//FINISHED: cd
//CURRENTLY WORKING ON: redirection
//GOAL: finish redirection
//NEXT GOAL: background

//prototypes
void shell_loop(char *input);
void rep_pid(char *input);
void parse(char *input, char **arguments, int *num_els);
void commands(char **arguments, int num_els);
void change_dir(char **arguments, int num_args);
bool is_redirect_exists(char **arguments, int num_els);
void redirect(char **arguments, int num_els, int *end, int *i_desc, int *o_desc);
void back_redirect();
void exec_nbcommands(char **arguments, int i_desc, int o_desc);

//global variables
bool ampersand_exists = false; //signifies if the process will be in the bg

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
    int end_index = 0; //ending index of args
    int fork_counter = 0;
    int ifile_desc = -1; //descriptors
    int ofile_desc = -1;

    do {
        if(fork_counter >= 9) {
            printf("too many forks breaking out of loop\n");
            fflush(stdout);
            break;
        }

        printf(": ");
        fflush(stdout);

        getline(&input, &bufsize, stdin);
        strtok(input, "\n"); //removes enter

        if (strlen(input) > TOT_CHARS) {
            printf("error too many characters\n");
            fflush(stdout);
        }

        else if (num_line_elements > TOT_ARGS) {
            printf("error too many arguments\n");
            fflush(stdout);
        }

        else { //run code
            rep_pid(input); //$$ to pid
            parse(input, args, &num_line_elements); //parse in arguments
            commands(args, num_line_elements); //finds built in commands

            printf("num els: %d\n", num_line_elements);
            fflush(stdout);

            if(is_redirect_exists(args, num_line_elements) == true) {
                //fork(); // must fork before redirection stuff, and make a counter to make sure you don't have more than 10 forks
                redirect(args, num_line_elements, &end_index, &ifile_desc, &ofile_desc);
                exec_nbcommands(args, ifile_desc, ofile_desc);
                fork_counter++;
            }

            printf("Number of elements: %d\n", num_line_elements);
            fflush(stdout);
            printf("Ending index: %d\n", end_index);
            fflush(stdout);
            printf("Input file descriptor: %d\n", ifile_desc);
            fflush(stdout);
            printf("Output file descriptor: %d\n", ofile_desc);
            fflush(stdout);

            //copy into new_args from args
            //for ()

            printf("Last argument index is %d\n", end_index);
            fflush(stdout); //clears stdout buffer 
        }

        fflush(stdout); //clears stdout buffer 

        num_line_elements = 0; //reset
        //stat = false; //when exiting turn stat to false
    } while(stat); //when stat is false it breaks otherwise keeps going
}

void redirect(char **arguments, int num_els, int *end, int *i_desc, int *o_desc) {

    printf("num els: %d\n", num_els);

    for(int index = num_els - 1; index >= 0; index--) {
        printf("index: %d\n", index);

        if(strcmp(arguments[index], "&") == 0) {
            ampersand_exists = true;
            //num_line_elements--; //starts at count 1
            //run bg version of redirection
        }

        else {
            if(strcmp(arguments[index], "<") == 0) { //reading in

                *i_desc = open(arguments[index + 1], O_RDONLY); //replace with open not fopen look at Aish's note in phone, int is used to pass through dup2()
                printf("Input file desc: %d\n", *i_desc);
                fflush(stdout);

                *end = index - 1;
                //dup2(i_desc, 0);
            }

            else if(strcmp(arguments[index], ">") == 0) { //writing out
                
                *o_desc = open(arguments[index + 1], O_WRONLY | O_TRUNC | O_CREAT, 0770);
                printf("Output file desc: %d\n", *o_desc);
                fflush(stdout);

                *end = index - 1;
                // printf("1\n");
                // fflush(stdout);
                //dup2(o_desc, 1);
                // printf("2\n");
                // fflush(stdout);
            }
            
            for(int k = end + 1; k < num_els; k++) {
                arguments[k] = NULL;
            }

            for (int i = 0; i < 10; i++) { //sees the arguments
                printf("%s ", arguments[i]);
                fflush(stdout);
            }
            printf ("\n");
            fflush(stdout);

        }
    }
}

void exec_nbcommands(char **arguments, int i_desc, int o_desc) {
    if ((i_desc >= 0) && (o_desc >= 0)) {
        execvp(arguments[0], arguments);
    }

    else {
        printf("error files not open\n");
        fflush(stdout);
    }
}

bool is_redirect_exists(char **arguments, int num_els) {
    bool read_red = false;
    bool out_red = false;

    for (int i  = 0; i < num_els; i++) {
        if (strcmp(arguments[i], "<") == 0) { //read in exists
            read_red = true;
        }

        else if (strcmp(arguments[i], ">") == 0) { //read out exists
            out_red = true;
        }
    }

    if ((read_red = true) || (out_red = true)) {
        return true;
    }

    return false;
}

void commands(char **arguments, int num_els) { //3 built-in commands
    int command_args = num_els - 1; //not include command itself

    if(strcmp(arguments[0], "cd") == 0) {
        printf("changing directories\n");
        fflush(stdout);
        change_dir(arguments, command_args);
    }

    else if(strcmp(arguments[0], "status") == 0) {
        printf("showing status\n");
        fflush(stdout);
    }

    else if(strcmp(arguments[0], "exit") == 0) {
        printf("exiting now\n");
        fflush(stdout);
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
        fflush(stdout);
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
                fflush(stdout);
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
        fflush(stdout);
    }
}