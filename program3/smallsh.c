/*
* Program name: Program 3 - smallsh
* Author: Joelle Perez
* Description: A program that runs as a mini shell.
* Date: 22 November 2019 (Less than 48 hours late)
*/ 
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

#define TOT_ARGS 512
#define TOT_CHARS 2048

//global variables
struct sigaction SIGINT_action;
struct sigaction SIGUSR2_action;
bool ampersand_exists = false; //signifies if the process will be in the bg
int pids_counter = 0;
pid_t bg_pids[20];
bool bg_allowed = true;

//prototypes
void shell_loop(char *input);
void rep_pid(char *input);
void parse(char *input, char **arguments, int *num_els);
void commands(char **arguments, int num_els, int *end, int *i_desc, int *o_desc, int *exit_status);
void change_dir(char **arguments, int num_args);
void show_status(int exit_status);
void exit_now();
bool is_redirect_exists(char **arguments, int num_els);
void redirect(char **arguments, int num_els, int *end, int *i_desc, int *o_desc);
void back_redirect(char **arguments, int num_els, int *end, int *i_desc, int *o_desc);
void switch_pids(char **arguments, int num_els, int *end, int *i_desc, int *o_desc, int *exit_status);
void catchSIGUSR2(int signo);

int main() {
    char *c_line;
    shell_loop(c_line); //get user input
    return 0;
}

/*
* Function name: shell_loop()
* Purpose: Prompts the user for input
* Arguments: char *
* Returns: none
* Source: Lecture 3.3 Signals
*/
void shell_loop(char *input) {
    ssize_t bufsize = 0; //initial 0
    bool stat = true; //keeps do while loop going until it exits
    int status = 0;
    char **args = malloc(TOT_ARGS * sizeof(char *));
    int num_line_elements = 0;
    int end_index = 0; //ending index of args
    int ifile_desc = -1; //descriptors
    int ofile_desc = -1;
    bool in_built = false;
    pid_t spawnpid;

    //for ctrlC - killing lec 3.3 signals
    SIGINT_action.sa_handler = SIG_IGN;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = SA_RESTART;
    sigaction(SIGINT, &SIGINT_action, NULL);

    //for ctrlZ - stopping lec 3.3 signals
    SIGUSR2_action.sa_handler = catchSIGUSR2;
    sigfillset(&SIGUSR2_action.sa_mask);
    SIGUSR2_action.sa_flags = 0;
    sigaction(SIGUSR2, &SIGUSR2_action, NULL);

    //do while loop for prompt
    do {
        if(pids_counter >= 20) {
            printf("too many forks breaking out of loop\n");
            fflush(stdout);
            break;
        }

        printf(": ");
        fflush(stdout);

        getline(&input, &bufsize, stdin);
        strtok(input, "\n"); //removes enter

        if(input[0] == '#') { //checks string for # char if so ignore
            continue;
        }

        if (strlen(input) > TOT_CHARS) { //checks for amount of chars
            printf("error too many characters\n");
            fflush(stdout);
        }

        else if (num_line_elements > TOT_ARGS) { //checks for amount of args
            printf("error too many arguments\n");
            fflush(stdout);
        }

        else { //run code
            rep_pid(input); //$$ to pid
            parse(input, args, &num_line_elements); //parse in arguments
            commands(args, num_line_elements, &end_index, &ifile_desc, &ofile_desc, &status); //finds built in commands
        }

        //reset array to empty nulls
        int i;
        for(i = 0; i < num_line_elements; i++) {
            args[i] = NULL;
        }
        num_line_elements = 0;

        //shows status if a child process has ended
        int j = 0;
        int pid_id;
        while((pid_id = waitpid(-1, &status, WNOHANG)) > 0) {
            printf("background pid %d is done: ", pid_id);
            fflush(stdout);
            show_status(status);
        }
    } while(stat); //when stat is false it breaks otherwise keeps going
}

/*
* Function name: switch_pids()
* Purpose: Forks a child and handles functions for that child and parent
* Arguments: char **, int, referenced integers
* Returns: none
* Source: Lecture 3.2 Process Management & Zombies
*/
void switch_pids(char **arguments, int num_els, int *end, int *i_desc, int *o_desc, int *exit_status) {
    signal(SIGTSTP, SIG_IGN); //ignores signals first
    pid_t spawnpid = fork();
    int exec_ret;

    switch(spawnpid)
    {
        case -1: //error
            perror("error fork gone wrong");
            exit(1);
            break;
        
        case 0: //child takes care of the non built in commands
            if((ampersand_exists) && (bg_allowed)) { //if bg and allowed to be fg
                redirect(arguments, num_els, end, i_desc, o_desc);
            }

            else if((ampersand_exists) && (is_redirect_exists(arguments, num_els) == true)) { //if background and has redirecretion
                back_redirect(arguments, num_els, end, i_desc, o_desc);
            }

            else if(is_redirect_exists(arguments, num_els) == true) { //if not background run foreground redirection
                SIGINT_action.sa_handler = SIG_DFL; //changes to default
                sigaction(SIGINT, &SIGINT_action, NULL);
                redirect(arguments, num_els, end, i_desc, o_desc);
            }

            else if((ampersand_exists == false) && (is_redirect_exists(arguments, num_els) == false)) { //if background and redirection does not exists
                SIGINT_action.sa_handler = SIG_DFL; //changes to default
                sigaction(SIGINT, &SIGINT_action, NULL);
            }

            exec_ret = execvp(arguments[0], arguments); //for non built in
            if(exec_ret < 0) {
                char *str = arguments[0];
                perror(str);
                exit(2);
            }
            break;
        
        default: //parent takes care of the built in commands
            signal(SIGTSTP, catchSIGUSR2);
            if(ampersand_exists == false) {
                waitpid(spawnpid, exit_status, 0);
            }
            else {
                printf("background pid is %d\n", spawnpid);
                pids_counter++;
                bg_pids[pids_counter - 1] = spawnpid; //captures pids in array
                waitpid(spawnpid, exit_status, WNOHANG);
            }
    }
}
/*
* Function name: catchSIGSUSR2
* Purpose: Catches ^Z signal
* Arguments: int
* Returns: none
* Source: Lecture 3.3 Signals
*/
void catchSIGUSR2(int signo) {
    char *message;
    if(bg_allowed == false) {
        message = "Exiting foreground-only mode\n";
        write(STDOUT_FILENO, message, 48);
        bg_allowed = true;
    }

    else if (bg_allowed) {
        message = "Entering foreground-only mode (& is now ignored)\n";
        write(STDOUT_FILENO, message, 28);
        bg_allowed = false;
    }
}

/*
* Function name: redirect()
* Purpose: Gets input files and creates output files
* Arguments: char **, int, referenced integers
* Returns: none
*/
void redirect(char **arguments, int num_els, int *end, int *i_desc, int *o_desc) {
    int index;
    for(index = num_els - 1; index >= 0; index--) {

        if(strcmp(arguments[index], "<") == 0) { //reading in file
            *i_desc = open(arguments[index + 1], O_RDONLY); //read
            if(*i_desc < 0) {
                printf("cannot open %s for input\n", arguments[index + 1]);
                fflush(stdout);
                exit(1);
            }
            *end = index - 1; //finds the last argument
            dup2(*i_desc, 0);
        }

        else if(strcmp(arguments[index], ">") == 0) { //writing out in file
            *o_desc = open(arguments[index + 1], O_WRONLY | O_TRUNC | O_CREAT, 0770); //write, truncate, create
            if(*o_desc < 0) {
                printf("cannot create %s for output\n", arguments[index + 1]);
                fflush(stdout);
                exit(1);
            }
            *end = index - 1; //finds last argument
            dup2(*o_desc, 1);
        }
    }

    // makes the new array for execvp
    int k;
    for(k = *end + 1; k < num_els; k++) {
        arguments[k] = NULL;
    }
}

/*
* Function name: back_redirect()
* Purpose: Redirection version for background
* Arguments: char **, int, referenced integers
* Returns: none
*/
void back_redirect(char **arguments, int num_els, int *end, int *i_desc, int *o_desc) {
    int index;
    for(index = num_els - 1; index >= 0; index--) {

        if(strcmp(arguments[index], "<") == 0) { //reading in
            if(arguments[index + 1] == NULL) {
                *i_desc = open("/dev/NULL", 0);
                dup2(*i_desc, 0);
            }
            *end = index - 1; //finds last arg
        }

        else if(strcmp(arguments[index], ">") == 0) { //writing out
            if(arguments[index + 1] == NULL) {
                *o_desc = open("/dev/NULL", 0);
                dup2(*o_desc, 0);
            }
            *end = index - 1; //finds last arg
        }
    }

    // makes the new array for execvp
    int k;
    for(k = *end + 1; k < num_els; k++) {
        arguments[k] = NULL;
    }
}

/*
* Function name: is_redirect_exists()
* Purpose: Checks if a redirection arrow exists in the array
* Arguments: char **, int
* Returns: boolean
*/
bool is_redirect_exists(char **arguments, int num_els) {
    bool read_red = false;
    bool out_red = false;

    int i;
    for (i  = 0; i < num_els; i++) {
        if (strcmp(arguments[i], "<") == 0) { //read in exists
            read_red = true;
        }

        else if (strcmp(arguments[i], ">") == 0) { //read out exists
            out_red = true;
        }
    }

    if ((read_red == true) || (out_red == true)) { //if eitherth are true then return true
        return true;
    }

    return false;
}

/*
* Function name: commands()
* Purpose: Finds built in commands and runs them
* Arguments: char **, int, referenced integers
* Returns: none
*/
void commands(char **arguments, int num_els, int *end, int *i_desc, int *o_desc, int *exit_status) { //3 built-in commands
    int command_args = num_els - 1; //not include command itself

    if(strcmp(arguments[0], "cd") == 0) { //change directory
        change_dir(arguments, command_args);
    }

    else if(strcmp(arguments[0], "status") == 0) { //print status
        show_status(*exit_status);
    }

    else if(strcmp(arguments[0], "exit") == 0) { //exit shell
        exit_now();
    }

    else { // non built in commands
        *end = num_els - 1;
        if(strcmp(arguments[*end], "&") == 0) { //finds out if bg then count for pids
            ampersand_exists = true;
            arguments[*end] = NULL; //makes & into NULL
            num_els--;
        }
        
        else {
            ampersand_exists = false; //is not bg
        }
        switch_pids(arguments, num_els, end, i_desc, o_desc, exit_status); //makes fork
    }
}

/*
* Function name: exit_now()
* Purpose: Kills and children and exits foreground
* Arguments: none
* Returns: none
*/
void exit_now() {
    int i;
    for (i = 0; i < pids_counter; i++) { //go through array and find pids and kill children
        kill(bg_pids[i], SIGKILL);
    }
    exit(0);
}

/*
* Function name: show_status()
* Purpose: Shows the exit value and terminated value for process
* Arguments: int
* Returns: none
*/
void show_status(int exit_status) {
    int exit_stat = WEXITSTATUS(exit_status);
    int term_sig = WTERMSIG(exit_status);

    if (WIFEXITED(exit_status) != 0) { //checks for exit
        printf("exit value %d\n", exit_stat);
    }

    if(WIFSIGNALED(exit_status) != 0) { //checks if terminated by signal
        printf("terminated by signal %d\n", term_sig);
    }
}

/*
* Function name: change_dir()
* Purpose: Changes direcory to new directory
* Arguments: char **, int
* Returns: none
*/
void change_dir(char **arguments, int num_args) {
    char path_dir[256];
    char new_cwd[256];
    char *cur_dir;

    if(num_args == 0) { //go to home dir when no args after command
        strcpy(path_dir, getenv("HOME"));
        chdir(path_dir);
    }

    else if(num_args == 1) { //for absolute and relative paths
        strcpy(path_dir, arguments[1]);
        chdir(path_dir);
    }

    else {
        printf("error too many arguments for cd\n");
        fflush(stdout);
    }

    cur_dir = getcwd(new_cwd, sizeof(new_cwd)); //gets new directory
}

/*
* Function name: parse()
* Purpose: Parses through string to place elements into array for later use
* Arguments: char *, char **, referenced int
* Returns: none
*/
void parse(char *input, char **arguments, int *num_els) {
        int i = 0;
        int elements = 0;
        char *arg = strtok(input, " ");

        while(arg != NULL) { //there's more elements
            if(i > TOT_ARGS) { //checks if there's too many arguments
                printf("error too many arguments");
                fflush(stdout);
                break;
            }

            arguments[i] = arg;
            i++; //increments count
            elements++; //increaments count for referenced var

            arg = strtok(NULL, " ");
        }
        *num_els = elements; //dereference passed arg to change to actual elements
}

/*
* Function name: rep_pid()
* Purpose: Changes && to pid when inputted
* Arguments: char *
* Returns: none
*/
void rep_pid(char *input) { //changes $$ to pid
    char new_str[300];
    char *dd = strstr(input, "$$");

    if(dd != NULL) {
        dd[0] = '%'; //takes the first char
        dd[1] = 'd'; //and second and change both to %d
        sprintf(new_str, input, getpid()); //to print out pid
        strcpy(input, new_str);
        printf("%s\n", input);
        fflush(stdout);
    }
}
