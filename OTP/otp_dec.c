#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFSIZE 1024 //Sets buffer size

//Prints out error message
void error(const char * msg) 
{
    perror(msg);
    exit(0);
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
        fprintf(stderr, "Usage: %s ciphertext key port\n", argv[0]);
        exit(1);
    }

    //Type cast port argument to an integer and set to portno
    portno = atoi(argv[3]);
    //Creates socket in address family INET
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //If -1 is returned from socket() print out error message
    if (sockfd < 0) {
        perror("Error opening socket.\n");
        exit(1); 
    }

    bzero((char *) &serv_addr, sizeof(serv_addr)); //Clear buffer for server address
    serv_addr.sin_family = AF_INET; //Sets address family as internet
    serv_addr.sin_port = htons(portno); //Only listen to specified port
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //Accept connection from any machine

    //Initiates a connection on a socket
    //If return value is less than 0, print that otp_dec_d could not be found
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("Error: otp_dec could not find otp_dec_d.\n");
        exit(2);
    }

    //Send plaintext file name
    snprintf(buffer, BUFSIZE, "%s", argv[1]);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        error("Error writing argv[1] to socket.\n");
        exit(1);
    }

    sleep(1);

    //Senf key file name
    snprintf(buffer, BUFSIZE, "%s", argv[2]);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        error("Error writing to socket.\n");
        exit(1);
    }

    //Receive response from server
    bzero(buffer, BUFSIZE);
    n = read(sockfd, buffer, BUFSIZE); //Waiting for server response
    if (n < 0) {
        error("Error reading from socket.\n");
        exit(1);
    }

    printf("%s\n", buffer);
    close(sockfd);

	return 0;
}
