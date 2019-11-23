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

//FINISHED: background
//CURRENTLY WORKING ON: signals
//GOAL: finish signals
//NEXT GOAL: zip smallsh.c with README.txt

// struct sigaction
// {
//     void (*sa_handler)(int);
//     sigset_t sa_mask;
//     int sa_flags;
//     void (*sa_sigaction)(int, siginfo_t*, void*);
// };

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

    //for ctrlC - killing
    SIGINT_action.sa_handler = SIG_IGN;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = SA_RESTART;
    sigaction(SIGINT, &SIGINT_action, NULL);

    //for ctrlZ - stopping
    SIGUSR2_action.sa_handler = catchSIGUSR2;
    sigfillset(&SIGUSR2_action.sa_mask);
    SIGUSR2_action.sa_flags = 0;
    sigaction(SIGUSR2, &SIGUSR2_action, NULL);

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
            // int exit_stat = WEXITSTATUS(status);
            // int term_sig = WTERMSIG(status);

            printf("background pid %d is done: ", pid_id);
            fflush(stdout);

            // if (WIFEXITED(status) != 0) { //checks for exit
            //     printf("exit value %d\n", exit_stat);
            // }

            // if(WIFSIGNALED(status) != 0) { //checks if terminated by signal
            //     printf("terminated by signal %d\n", term_sig);
            // }
            show_status(status);
        }
    } while(stat); //when stat is false it breaks otherwise keeps going
}

void switch_pids(char **arguments, int num_els, int *end, int *i_desc, int *o_desc, int *exit_status) {
    signal(SIGTSTP, SIG_IGN);
    pid_t spawnpid = fork();
    int exec_ret;

    switch(spawnpid)
    {
        case -1: //error
            perror("error fork gone wrong");
            exit(1);
            break;
        
        case 0: //child takes care of the non built in commands
            if((ampersand_exists) && (bg_allowed)) {
                back_redirect(arguments, num_els, end, i_desc, o_desc);
            }

            else if((ampersand_exists) && (is_redirect_exists(arguments, num_els) == true)) { //if background and has redirecretion
                back_redirect(arguments, num_els, end, i_desc, o_desc);
            }

            else if(is_redirect_exists(arguments, num_els) == true) { //if not background run foreground redirection
                SIGINT_action.sa_handler = SIG_DFL;
                sigaction(SIGINT, &SIGINT_action, NULL);
                redirect(arguments, num_els, end, i_desc, o_desc);
            }

            else if((ampersand_exists == false) && (is_redirect_exists(arguments, num_els) == false)) { //if background and redirection does not exists
                SIGINT_action.sa_handler = SIG_DFL;
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
                bg_pids[pids_counter - 1] = spawnpid;
                waitpid(spawnpid, exit_status, WNOHANG);
            }
    }
}

void catchSIGUSR2(int signo) {
    char *message;
    if(bg_allowed == false) {
        message = "Exiting foreground-only mode";
        write(STDOUT_FILENO, message, 48);
        bg_allowed = true;
    }

    else if (bg_allowed) {
        message = "Entering foreground-only mode (& is now ignored)";
        write(STDOUT_FILENO, message, 28);
        bg_allowed = false;
    }
}

void redirect(char **arguments, int num_els, int *end, int *i_desc, int *o_desc) {
    int index;
    for(index = num_els - 1; index >= 0; index--) {

        if(strcmp(arguments[index], "<") == 0) { //reading in
            *i_desc = open(arguments[index + 1], O_RDONLY);
            if(*i_desc < 0) {
                printf("cannot open %s for input\n", arguments[index + 1]);
                fflush(stdout);
                exit(1);
            }
            *end = index - 1;
            dup2(*i_desc, 0);
        }

        else if(strcmp(arguments[index], ">") == 0) { //writing out
            *o_desc = open(arguments[index + 1], O_WRONLY | O_TRUNC | O_CREAT, 0770);
            if(*o_desc < 0) {
                printf("cannot create %s for output\n", arguments[index + 1]);
                fflush(stdout);
                exit(1);
            }
            *end = index - 1;
            dup2(*o_desc, 1);
        }
    }

    // makes the new array for execvp
    int k;
    for(k = *end + 1; k < num_els; k++) {
        arguments[k] = NULL;
    }
}

void back_redirect(char **arguments, int num_els, int *end, int *i_desc, int *o_desc) {
    int index;
    for(index = num_els - 1; index >= 0; index--) {

        if(strcmp(arguments[index], "<") == 0) { //reading in
            if(arguments[index + 1] == NULL) {
                *i_desc = open("/dev/NULL", 0);
                dup2(*i_desc, 0);
            }
            *end = index - 1;
        }

        else if(strcmp(arguments[index], ">") == 0) { //writing out
            if(arguments[index + 1] == NULL) {
                *o_desc = open("/dev/NULL", 0);
                dup2(*o_desc, 0);
            }
            *end = index - 1;
        }
    }

    // makes the new array for execvp
    int k;
    for(k = *end + 1; k < num_els; k++) {
        arguments[k] = NULL;
    }
}

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

    if ((read_red == true) || (out_red == true)) {
        return true;
    }

    return false;
}

void commands(char **arguments, int num_els, int *end, int *i_desc, int *o_desc, int *exit_status) { //3 built-in commands
    int command_args = num_els - 1; //not include command itself

    if(strcmp(arguments[0], "cd") == 0) {
        change_dir(arguments, command_args);
    }

    else if(strcmp(arguments[0], "status") == 0) {
        show_status(*exit_status);
    }

    else if(strcmp(arguments[0], "exit") == 0) {
        exit_now();
    }

    else { // non built in commands
        *end = num_els - 1;
        if(strcmp(arguments[*end], "&") == 0) { //finds out if bg then count for pids
            ampersand_exists = true;
            arguments[*end] = NULL;
            num_els--;
        }
        
        else {
            ampersand_exists = false;
        }
        switch_pids(arguments, num_els, end, i_desc, o_desc, exit_status);
    }
}

void exit_now() {
    int i;
    for (i = 0; i < pids_counter; i++) {
        kill(bg_pids[i], SIGKILL);
    }
    exit(0);
}

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

    cur_dir = getcwd(new_cwd, sizeof(new_cwd));
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
