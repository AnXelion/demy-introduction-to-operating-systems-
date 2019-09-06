#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for memset()
#include <unistd.h> // for close()

// Constants
#define SERVER_ADDR "localhost"
#define SERVER_PORT 8888
#define MESSAGE "Hello, wOrlD"
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    
    int socketFD = 0;
    struct sockaddr_in serverSocketAddr;
    char buffer[BUFFER_SIZE];
    
    // Create socket (IPv4, stream-based, protocol likely set to TCP)
    if (-1 == (socketFD = socket(PF_INET, SOCK_STREAM, 0))) {
        perror("Client failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Convert localhost into 0.0.0.0
    struct hostent *he = gethostbyname(SERVER_ADDR);
    unsigned long serverAddrNBO = *(unsigned long *)(he->h_addr_list[0]);

    // Configure server socket address structure (init to zero, IPv4,
    // network byte order for port and address) 
    // (Description copied from the solution code)
    memset(&serverSocketAddr, 0, sizeof(struct sockaddr_in));
    serverSocketAddr.sin_family = AF_INET;
    serverSocketAddr.sin_port = SERVER_PORT;
    serverSocketAddr.sin_addr.s_addr = serverAddrNBO;

    // Connect socket to server
    if (-1 == connect(socketFD, (struct sockaddr *) &serverSocketAddr, sizeof(serverSocketAddr))) {
        // Failure
        fprintf(stderr, "Client failed to connect to %s:%d\n", SERVER_ADDR, SERVER_PORT);   
        close(socketFD);
        exit(EXIT_FAILURE);
    }
    else {
        // Success
        fprintf(stdout, "Client connected to %s:%d\n", SERVER_ADDR, SERVER_PORT);
    }

    // Send echo message to the server
    if (-1 ==  send(socketFD,  MESSAGE, strlen(MESSAGE), 0)) {
        fprintf(stderr, "Client falied to send echo message\n");
        close(socketFD);
        exit(EXIT_FAILURE);
    }

    // Process response message from the server
    if (-1 == read(socketFD, buffer, BUFFER_SIZE)) {
        fprintf(stderr, "Client could not read response from the server\n");
        close(socketFD);
        exit(EXIT_FAILURE);
    }
    else {
        fprintf(stdout, "Echo from the server: %s\n", buffer);
    }

    close(socketFD);
    return 0;
}