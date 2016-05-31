#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFSIZE 1024 //Sets buffer size

//Prints out error message
void error(const char *msg) 
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) 
{
    //Setup socket file descriptor and port number
    int sockfd, portno, n; 
    struct sockaddr_in serv_addr;
    char buffer[BUFSIZE];

    //Checks if user supplies less than four arguments
    //If so, prints usage message
    if (argc < 4) {
       fprintf(stderr,"Usage: %s plaintext key port\n", argv[0]);
       exit(1);
    }

    //Type cast port argument to an integer and set to portno
    portno = atoi(argv[3]);
    //Creates socket in address family INET
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //If -1 is returned from socket() print out error message
    if (sockfd < 0) {
        error("ERROR opening socket.\n");
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr)); //Clear buffer for server address
    serv_addr.sin_family = AF_INET; //Sets address family as internet
    serv_addr.sin_port = htons(portno); //Only listen to specified port 
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //Accept connection from any machine

    //Initiates a connection on a socket
    //If return value is -1, print error connecting 
    if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("ERROR connecting to port %d.\n", portno);
        exit(2);
    }

    //Send plaintext file name
    snprintf(buffer, BUFSIZE, "%s", argv[1]);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        error("ERROR writing argv[1] to socket.\n");
    }

    sleep(1);

    //Send key file name
    snprintf(buffer, BUFSIZE, "%s", argv[2]);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        error("ERROR writing to socket.\n");
    }

    //Receive response from server
    bzero(buffer, BUFSIZE);
    n = read(sockfd, buffer, BUFSIZE); //Waiting for server response
    if (n < 0) {
        error("ERROR reading from socket.\n");
    }

    if (buffer == "Error: key too short") {
        error("ERROR Key not long enough.\n");
    } else if (buffer == "error2") {
        error("ERROR Bad characters in file.\n");
    } else {
        printf("%s\n", buffer);
    }

    close(sockfd);
     
    return 0;
}