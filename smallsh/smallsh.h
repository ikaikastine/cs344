#ifndef SMALLSH_H
#define SMALLSH_H

//Define statements 
#define TOK_DELIM " \t\r\n\a"
#define bufferSize 1024

//Various header definitions
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

//Declaration of global variables
int numArgs, backProcess;

//Function prototypes
char *getLine();

char **splitLine(char *line);

int execute(char **args);

void handle_SIGINT();

void handle_SIGTERM();

#endif
