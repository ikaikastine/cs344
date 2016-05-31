#include "smallsh.h"

int main()
{
	int exitCalled = 0; //Used to know if the program should exit
	int status = 0; //Used to know the status (0 = success, 1 = failure)

	do {
		printf(": "); //Prints the prompt

		//Declaration of variables used
		char line[bufferSize];
		size_t size = 0;
		char **args;
		int fileDescriptor;
		numArgs = 0;
		backProcess = 0;
		pid_t pid;

		fgets(line, bufferSize, stdin); //Reads input from user and stores in line
		args = splitLine(line); //Store extracted tokens in args array

		//Check to see if process is running in the background
		//If it is, mark as backProcess 
		if (!(strncmp(args[numArgs - 1], "&", 1))) {
			pid = getpid();
			backProcess = 1; //Marks as background process
			args[numArgs - 1] = NULL; //Clear token 
		}

		//Check to see if input is blank or a comment
		//If the input is empty it segfaults for some reason
		//Really not sure why
		if (args[0] == NULL || !(strncmp(args[0], "#", 1))) {
			status = 0; //success
		}

		//Builtin "cd" command 
		//Compares first argument to cd
		else if (strcmp(args[0], "cd") == 0) {
			//Check to see if there is only one argument
			if(args[1]) {
				//Move to the directory specified by the user
				//If the directory does not exist, print error
				if(chdir(args[1]) != 0) {
					printf("%s: No such file or directory\n", args[1]);
					status = 1; //failed to find file
				}
			}
			//If the user does not specify a directory
			//Default to home
			else {
				chdir(getenv("HOME"));
				status = 0;
			}
		}

		//Builtin "exit" command
		//Compares first argument to exit
		else if(strcmp(args[0], "exit") == 0) {
			exitCalled = 1; //If they match, set exitCalled to 1 to quit
			status = 0;
		}

		//Builtin "status" command
		//Compares first argument to status
		else if(strcmp(args[0], "status") == 0) {
			//Sets status if interrupted by SIGINT
			if (status == 2) {
				printf("Terminated by signal 2\n");
				status = 0;
			}
			//Sets status if termianted by SIGTERM
			if (status == 3) {
				printf("Terminated by signal 15\n");
				status = 0;
			}
			//Prints the status, then reset it to zero
			else {
				printf("Exit status: %d\n", status);
				status = 0;
			}
		}

		//Checks for redirection in input command
		else if (numArgs == 3 && ((strcmp(args[1], ">") == 0) || (strcmp(args[1], "<") == 0) )) {
			int savedOutput, savedInput; //Declares two variables to store input & output
			savedInput = dup(0); //Creates a copy of file descriptor 
			savedOutput = dup(1); //Creates a copy of file descriptor

			//Checks to see if redirecting standard output
			if (strcmp(args[1], ">") == 0) {
				//Opens the file to direct standard output 
				//Opens for writing, creating and truncating 
				fileDescriptor = open(args[2], O_WRONLY|O_CREAT|O_TRUNC, 0644);
				//If file could not be successfully opened 
				if (fileDescriptor == -1) {
					printf("No such file or directory\n");
					status = 1; //Set status to failed
				}
				else {
					dup2(fileDescriptor, 1); //Points stdout to this file
					args[1] = NULL; //Terminate anything after first command
					close(fileDescriptor); //Close file 
					status = execute(args); //Execute the command 
				}
			}

			//Checks to see if redirecting standard input
			else if (strcmp(args[1], "<") == 0) {
				fileDescriptor = open(args[2], O_RDONLY); //Opens for reading only
				//If the file could not be succesfully opened
				if (fileDescriptor == -1) {
					printf("%s: No such file or directory\n", args[2]);
					status = 1;
				}
				else {
					dup2(fileDescriptor, 0); //Points stdin to this file
					args[1] = NULL; //Terminate anything after first command
					close(fileDescriptor); //Close file
					status = execute(args); //Execute the command
				}
			}
			dup2(savedOutput, 1); //Restore original output 
			close(savedOutput); 
			dup2(savedInput, 0); //Restore original input 
			close(savedInput);
		}
		
		//All other commands that are not built in
		else {
			status = execute(args); //Execute the command
		}
		free(args); //Deallocate args
	} while (!exitCalled); //Loop until exit is called
	return 0;
}