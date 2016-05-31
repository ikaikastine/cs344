#include "smallsh.h"
#include <signal.h>

//handles the signal SIGINT
void handle_SIGINT() 
{
    printf(" Terminated by signal 2\n"); //Catches CTRL-C and print newline 
    //exitStatus = 2;
}

//handles the signal SIGTERM
void handle_SIGTERM() 
{
    printf("Found your sigterm\n");
}

int execute(char **args) 
{
    pid_t pid, wpid;
    int status, exitStatus = 0;

    //Start signal catching
    struct sigaction handler; //Creates handler from struct sigaction
    struct sigaction action; //Creates action from struct sigaction
    handler.sa_handler = handle_SIGINT; 
    action.sa_handler = handle_SIGTERM;
    sigaction(SIGINT, &handler, NULL); //Handles a signal interrupt
    sigaction(SIGTERM, &action, NULL); //Handles a signal terminate

    pid = fork(); //Fork the current process, creating a new child process

    //The fork system call returns a zero to the newly created child process
    //Checks to see if the pid matches zero, if so it is the child
    if (pid == 0) {
        //Child process executes unless an error occurs and returns -1
        //If execvp returns -1, print error and quit child process
        if (execvp(args[0], args) == -1) { 
            printf("Command or file not recognized\n"); 
            exit(1);
        }
    }
    //The fork system call returns a negative value if the creation of the child
    //Was unsuccesful. If the pid is less than 0, print out the error
    else if (pid < 0) {
        perror("smallsh");
    }
    //No errors occured in the forking process, parent waits for child
    //to finish executing
    else {
        do {
            //Waits for child process to finish
            if (backProcess == 0) {
                wpid = waitpid(pid, &status, WUNTRACED); //Returns if child has stopped
            }
            //Waits for any child process to finish
            else if (backProcess == 1) {
                wpid = waitpid(-1, &status, WNOHANG); //Returns immediatly is no child has exited
            }  
        } 
        //Continues to wait until child process terminates normally
        //or until child terminates after receiving a signal 
        while (!WIFEXITED(status) && !WIFSIGNALED(status)); 
    }
    //Prints out child process' ID and status
    if (backProcess == 1) {
        printf("Background PID: %d\nExit Status: %d\n", pid, exitStatus);
    }
    //If the child was terminated due to a signal that was not caught 
    //set status to failed
    if (status != 0 || WIFSIGNALED(status)) {
        exitStatus = 1;
    }
    //Evaluates the number of the signal that caused the termination
    //of the child. Set status to failed
    else if (status != 0 || WTERMSIG(status)) {
        exitStatus = 1;
    }
    return exitStatus;
}

