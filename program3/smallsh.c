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

    do {
        printf(": ");
        getline(&input, &bufsize, stdin);
        strtok(input, "\n"); //removes enter

        if (strlen(input) > TOT_CHARS) {
            printf("error too many characters\n");
        }

        else if (num_line_elements > TOT_ARGS) {
            printf("error too many arguments\n");
        }

        else {
            rep_pid(input); //$$ to pid
            parse(input, args, &num_line_elements); //parse in arguments
            commands(args, num_line_elements); //finds built in commands

            
            //validate if redirection exists then
            // bool read_red = false;
            // bool out_red = false;
            // bool redirection = false;
            // for (int i  = 0; i < num_line_elements; i++) {
            //     if (strcmp(args[i], "<") == 0) { //read in exists
            //         read_red = true;
            //     }

            //     else if (strcmp(args[i], ">") == 0) { //read out exists
            //         out_red = true;
            //     }
            // }

            // if ((read_red = true) || (out_red = true)) {
            //     redirection = true;
            // }
            //fork(); // must forl before redirection stuff, and make a counter to make sure you don't have more than 10 forks

            //redirection
            // char *o_file = NULL;
            // char *i_file = NULL;
            // FILE *i_ptr;
            // FILE *o_ptr;
            int ifile_desc;
            int ofile_desc;
            int i_dup_ret;
            int o_dup_ret;
            int end_index = 0; //ending index of args
            int num_args = 0;

            printf("num els: %d\n", num_line_elements);

            for(int index = num_line_elements - 1; index >= 0; index--) {
                printf("index: %d\n", index);

                if(strcmp(args[index], "&") == 0) {
                    printf("am & gonna run in bg\n"); //run in background

                    ampersand_exists = true;
                    //num_line_elements--; //starts at count 1

                    //run bg version of redirection
                }

                else {
                    if(strcmp(args[index], "<") == 0) { //reading in

                        ifile_desc = open(args[index + 1], O_RDONLY); //replace with open not fopen look at Aish's note in phone, int is used to pass through dup2()
                        printf("Input file desc: %d\n", ifile_desc);

                        // for (int i = index; i < num_line_elements; i++) {
                        //     args[i] = NULL;
                        // }

                        end_index = index - 1;
                        i_dup_ret = dup2(ifile_desc, 0);
                        //  for (int i = index; i < num_line_elements; i++) {
                        //     args[i] = NULL;
                        // }
                    }

                    else if(strcmp(args[index], ">") == 0) { //writing out
                        
                        ofile_desc = open(args[index + 1], O_WRONLY | O_TRUNC | O_CREAT, 0644);
                        printf("Output file desc: %d\n", ofile_desc);
                        fflush(stdout);

                        // for (int i = index; i < num_line_elements; i++) {
                        //     args[i] = NULL;
                        // }

                        end_index = index - 1;
                        printf("1\n");
                        fflush(stdout);
                        o_dup_ret = dup2(ofile_desc, 1);
                        printf("2\n");
                        fflush(stdout);
                        //close(ofile_desc);

                        break;
                    }
                    // int k;
                    // for(k = index; k < num_line_elements; k++) {
                    //     args[k] = NULL;
                    // }

                    // for (int i = 0; i < 10; i++) { //sees the arguments
                    //     printf("%s ", args[i]);
                    // }
                    // printf ("\n");

                    //works, but needs end to be right
                    // if ((ifile_desc >= 0) && (ofile_desc >= 0)) {
                    //     execvp(args[0], args);
                    // }

                    // else {
                    //     printf("error files not open\n");
                    // }
                }
            }

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

bool is_redirect_exists(char **arguments, int num_els) {
    bool read_red = false;
    bool out_red = false;
    bool redirection = false;

    for (int i  = 0; i < num_els; i++) {
        if (strcmp(arguments[i], "<") == 0) { //read in exists
            read_red = true;
        }

        else if (strcmp(arguments[i], ">") == 0) { //read out exists
            out_red = true;
        }
    }

    if ((read_red = true) || (out_red = true)) {
        redirection = true;
    }

    return redirection;
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