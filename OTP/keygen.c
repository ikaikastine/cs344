#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) 
{
	//Checks to see if two arguments are supplied
	//If not, print out the correct usage for keygen
	if (argc != 2) {
		printf("Usage: %s length\n", argv[0]);
	}
	else {
		//Generate random number seed 
		srand(time(NULL));
		int i;
		//Convert argument one to an integer
		int len = atoi(argv[1]);
		char curChar, key[len+1];

		for (i = 0; i < len; i++) {
			//Select random letters from a string
			curChar = "ABCDEFGHIJKLMNOPQRSTUVQXYZ "[random() % 27];
			//Fills the key array with random letters
			key[i] = curChar;
		}
		//Null terminates the string
		key[len-1] = '\0';
		//Prints out the key string
		printf("%s\n", key);
	}
	return 0;
}
