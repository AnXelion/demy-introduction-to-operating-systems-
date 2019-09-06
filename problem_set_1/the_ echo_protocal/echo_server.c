#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for memset()
#include <ctype.h> // toupper()
#include <unistd.h> // for close()

// Constants
#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

void toupperBuffer(char *s) {
    while(*s) {
        *s = toupper(*s);
        s++;
    }
}

int main(int argc, char *argv[]) {
    
    int serverSocketFD = 0;
    int clientSocketFD = 0;
    struct sockaddr_in serverSocketAddr;
    struct sockaddr_in clientSocketAddr;

    int max_pending_connections = 1;
    
    struct hostent *clientHostInfo = NULL;
    char *clientHostIP;

    char buffer[BUFFER_SIZE];
    int num_bytes = 0;

    // Create a socket (IPv4, stream-based, protocol likely set to TCP)    
    if (-1 == (serverSocketFD = socket(PF_INET, SOCK_STREAM, 0))) {
        fprintf(stderr, "Server failed to create the listening socket\n");
        exit(EXIT_FAILURE);     
    }

    // Configure server socket address structure (init to zero, IPv4,
    // network byte order for port and address)
    memset(&serverSocketAddr, 0, sizeof(serverSocketAddr));
    serverSocketAddr.sin_family = AF_INET;
    serverSocketAddr.sin_port = htons(SERVER_PORT);
    serverSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket
    if (-1 == bind(serverSocketFD, (struct sockaddr*) &serverSocketAddr, sizeof(serverSocketAddr))) {
        fprintf(stderr, "Server failed to bind\n");
        exit(EXIT_FAILURE);   
    }

    // Listen on the socket for up to some maximum pending connections
    if (-1 == listen(serverSocketFD, max_pending_connections)) {
        fprintf(stderr, "Server failed to listen\n");
        close(EXIT_FAILURE);
    }
    else {
        fprintf(stdout, "Server is listening to a connection on port %d\n", SERVER_PORT);
    }

    // Fix error: lvalue required as unary ‘&’ operand
    int clientSocketAddrLen = (sizeof(clientSocketAddr));

    // Accept a new client
    if (-1 == (clientSocketFD = accept(serverSocketFD, (struct sockaddr*) &clientSocketAddr, &clientSocketAddrLen))) {
        fprintf(stderr, "Server accept failed\n");
    }
    else {
        fprintf(stderr, "Server accepted a client\n");
        // close(clientSocketFD); // Close the client socket
    }

    // Determine who sent the message
    clientHostInfo = gethostbyaddr((struct sockaddr_in *) &clientSocketAddr.sin_addr.s_addr, sizeof(clientSocketAddr.sin_addr.s_addr), AF_INET);
    
    // Cannot determine the client
    if (clientHostInfo == NULL) {
        fprintf(stderr, "Server could not determine client host address\n");
    }
    
    clientHostIP = inet_ntoa(clientSocketAddr.sin_addr);
    if (clientHostIP == NULL) {
        fprintf(stderr, "Server could not determine client IP\n");
    }
    fprintf(stdout, "Server established connection with %s: %s\n", clientHostInfo->h_name, clientHostIP);

    // Read echo from the client
    memset(&buffer, 0, BUFFER_SIZE);
    num_bytes = read(clientSocketFD, buffer, BUFFER_SIZE);
    if (num_bytes == 0) {
        fprintf(stderr, "Sever could not read from socket\n");
    }
    else {
        fprintf(stdout, "Server received %d bytes: %s\n", num_bytes, buffer);
    }

    // Modify client message
    toupperBuffer(buffer); 

    // Respond the client a modified message
    if (-1 == write(clientSocketFD, buffer, strlen(buffer))) {
        fprintf(stderr, "Server couldnot write back to socket\n");
    }
    else {
        fprintf(stdout, "Server is sending message back to client\n");
    }

    // Close the server socket
    close(serverSocketFD);
    return 0;
}