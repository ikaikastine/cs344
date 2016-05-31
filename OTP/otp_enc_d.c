#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h> //letter to number conversion

#define BUFSIZE 1024 //Sets buffer size

//Prints out error message
void error(const char *msg) {
    perror(msg);
    exit(1);
}

//Converts char values to integers
int convertChar(char character) 
{
    switch (character) {
        case 'A': return 0;
        case 'B': return 1;
        case 'C': return 2;
        case 'D': return 3;
        case 'E': return 4;
        case 'F': return 5;
        case 'G': return 6;
        case 'H': return 7;
        case 'I': return 8;
        case 'J': return 9;
        case 'K': return 10;
        case 'L': return 11;
        case 'M': return 12;
        case 'N': return 13;
        case 'O': return 14;
        case 'P': return 15;
        case 'Q': return 16;
        case 'R': return 17;
        case 'S': return 18;
        case 'T': return 19;
        case 'U': return 20;
        case 'V': return 21;
        case 'W': return 22;
        case 'X': return 23;
        case 'Y': return 24;
        case 'Z': return 25;
        case ' ': return 26;
    }
}

//Checks to make sure characters are valid
int validateChar(char character) 
{
    switch (character) {
        case 'A': return 0;
        case 'B': return 0;
        case 'C': return 0;
        case 'D': return 0;
        case 'E': return 0;
        case 'F': return 0;
        case 'G': return 0;
        case 'H': return 0;
        case 'I': return 0;
        case 'J': return 0;
        case 'K': return 0;
        case 'L': return 0;
        case 'M': return 0;
        case 'N': return 0;
        case 'O': return 0;
        case 'P': return 0;
        case 'Q': return 0;
        case 'R': return 0;
        case 'S': return 0;
        case 'T': return 0;
        case 'U': return 0;
        case 'V': return 0;
        case 'W': return 0;
        case 'X': return 0;
        case 'Y': return 0;
        case 'Z': return 0;
        case ' ': return 0;
        case '$': return 1;
        default : return 1;
    }
}

int main(int argc, char *argv[]) 
{
    int n, i; //Loop iterators
    int sockfd, acceptfd, portno; //Initialize socket file descriptors and port
    socklen_t clilen; //Initialize client address length
    char recvbuf[BUFSIZE], sendbuf[BUFSIZE]; //Initialize send/receive string buffers
    struct sockaddr_in serv_addr, cli_addr; //Initialize server and client structs
    

    //Checks for two arguments
    //If less, print out usage message
    if (argc < 2) { 
        fprintf(stderr,"Usage: %s port\n", argv[0]);
        exit(1);
    }

    pid_t pid, sid; //Initialize process ID and Session ID
    
    //Fork parent process
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    //If PID is good, exit parent process
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    //Change file mode so daemon can read/write files
    umask(0);   

    //Create new SID for child process
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    //Close out the standard file descriptors 
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    //Creates socket in address family INET
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd < 0) {
        error("ERROR opening socket.\n");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr)); //Clear buffer for server address
    portno = atoi(argv[1]); //Convert port argument to int and set port number
    serv_addr.sin_family = AF_INET; //Sets address family as internet
    serv_addr.sin_port = htons(portno); //Only listen to specified port 
    serv_addr.sin_addr.s_addr = INADDR_ANY; //Accept connections from any machine
    
    //Assign specified address space
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding.\n");
    }

    //Listen for up to 5 incoming connections on socket
    listen(sockfd, 5);
    clilen = sizeof(cli_addr); 

    //Beginning of the listening loop
    while (1) {
        //Wait for connection attempts from client processes
        acceptfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (acceptfd < 0) {
            error("ERROR on accept.\n");
        }

        //Read from client message one
        bzero(recvbuf, BUFSIZE); //Clear the string buffer for received messages
        n = read(acceptfd, recvbuf, BUFSIZE); //Read incoming messages
        if (n < 0) {
            error("ERROR reading from socket.\n");
        }

        //Attempt to open a file whose name was in the message
        FILE *fp;
        fp = fopen(recvbuf, "r");
        if (fp == NULL) {
            error("ERROR opening plaintext file.\n");
        }

        char plainText[BUFSIZE]; //This will store plaintext string to be extracted from file
        fgets(plainText, BUFSIZE, fp); //Read file, store into plainText

        for (i = 0; i < strlen(plainText); i++) { //Convert plaintext to all upper case
            plainText[i] = toupper(plainText[i]);
        }

        for (i = 0; i < strlen(plainText)-1; i++) { //Check for bad input
            if (validateChar(plainText[i]) == 1) { //If validateChar returns true, exit with error
                error("ERROR plaintext file contained bad characters.\n");
            }
        }

        fclose(fp);

        //Read from client message two
        bzero(recvbuf, BUFSIZE); //Clear the buffer so it can be safely reused
        n = read(acceptfd, recvbuf, BUFSIZE); //Read any incoming messages into the buffer
        if (n < 0) {
            error("ERROR reading from socket.\n");
        }

        //Attempt to open the file named in the message
        fp = fopen(recvbuf, "r");
        if (fp == NULL) {
            error("ERROR opening key file.\n");
        }

        char keyText[BUFSIZE]; //Stores key string to be extracted from file
        fgets(keyText, BUFSIZE, fp); //Read file, store contents into keyText

        fclose(fp);

        //Use keyText to encrypt plainText
        if (strlen(keyText) < strlen(plainText)) {
            bzero(sendbuf, BUFSIZE); //Clear the send buffer 
            snprintf(sendbuf, BUFSIZE, "%s", "Error: key too short"); //Store the cipher string into the buffer
            n = write(acceptfd, sendbuf, strlen(sendbuf)); //Write this message back to the client
            if (n < 0) {
                error("ERROR writing to socket.\n");
            }
            continue;
        }

        //Then encrypt each character
        char cipherText[strlen(plainText)]; //Stores cipherText
        //Store the strings as numbers 
        int plainVal[strlen(plainText)], keyVal[strlen(plainText)], cipherVal[strlen(plainText)];
        char letters[28] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "; 

        for (i = 0; i < strlen(plainText); i++) { 
            plainVal[i] = convertChar(plainText[i]); //Convert each character of plaintext to numbers 
            keyVal[i] = convertChar(keyText[i]); //Convert each character of keytext to numbers
            cipherVal[i] = plainVal[i] + keyVal[i]; //Cipherval is set as the sum of plainVal and keyVal
            //Modular addition if the cipherVal is over 26
            if (cipherVal[i] > 26) { 
                cipherVal[i] -= 27;
            }
            cipherText[i] = letters[cipherVal[i]]; //Convert cipher number to a letter
        }
        cipherText[strlen(plainText)-1] = '\0'; //Null terminate the resulting message

        //Write back to client
        bzero(sendbuf, BUFSIZE); 
        snprintf(sendbuf, BUFSIZE, "%s", cipherText); //Store the cipher 
        n = write(acceptfd, sendbuf, strlen(sendbuf)); //Write this message back to the client
        if (n < 0) {
            error("ERROR writing to socket.\n");
        }
    }
    return 0;
}