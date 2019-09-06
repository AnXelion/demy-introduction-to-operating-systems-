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
#define SERVER_PORT 8888


int main(int argc, int *argv[]) {

    int socketFD = 0;
    int clientSocketFD = 0;
    struct sockaddr_in serverSocketAddr;
    struct sockaddr_in clientSocketAddr;

    int max_pending_connections = 1;

    // Create a socket (IPv4, stream-based, protocol likely set to TCP)
    if (-1 == (socketFD = socket(PF_INET, SOCK_STREAM, 0))) {
        fprintf(stderr, "Server failed to create the listening socket\n");
        exit(EXIT_FAILURE);
    }

    // Configure server socket address structure (init to zero, IPv4,
    // network byte order for port and address)
    memset(&serverSocketAddr, 0, sizeof(struct sockaddr_in));
    serverSocketAddr.sin_family = AF_INET;
    serverSocketAddr.sin_port = htons(SERVER_PORT);
    serverSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket
    if (-1 == bind(socketFD, (struct sockaddr *) &serverSocketAddr, sizeof(serverSocketAddr))) {
        fprintf(stderr, "Server failed to bind\n");
        exit(EXIT_FAILURE);
    }

    // Listen on the socket for up to some maximum pending connections
    if (-1 == listen(socketFD, max_pending_connections)) {
        fprintf(stderr, "Server failed to listen\n");
        exit(EXIT_FAILURE);
    }
    else {
        fprintf(stdout, "Server is listening for a connection on port %d\n", SERVER_PORT);
    }


    // Fix error: lvalue required as unary ‘&’ operand
    int clientSocketAddrLen = (sizeof(clientSocketAddr));

    // Accept a new client
    if (-1 == (clientSocketFD = accept(socketFD, (struct sockaddr *) &clientSocketAddr, &clientSocketAddrLen))) {
        fprintf(stderr, "Server accept failed\n");
    }
    else {
        fprintf(stderr, "Server accepted a client\n");
        close(clientSocketFD); // Close the client socket
    }

    // Close the server socket
    close(socketFD);
    return 0;
}