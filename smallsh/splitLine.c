#include "smallsh.h"

//Tokenize the command line input into multiple sections
char **splitLine(char *line) 
{
    int buffer = 64;
    int pos = 0;
    char **tokens = malloc(buffer * sizeof(char*));
    char *token;
    token = strtok(line, TOK_DELIM); //Grab the first argument string
    //Grab all of the elements until the end 
    while (token != NULL) {
        tokens[pos] = token; //Store current token in an array
        numArgs++; //Increment total arguments 
        pos++; 
        //If the buffer reaches the end, add more space
        if (pos >= buffer) {
            buffer += 64;
            tokens = realloc(tokens, buffer * sizeof(char*));
        }
        token = strtok(NULL, TOK_DELIM); //Increment until next deliminator 
    }
    tokens[pos] = NULL; //Drops a null at the end of the array
    return tokens;
}