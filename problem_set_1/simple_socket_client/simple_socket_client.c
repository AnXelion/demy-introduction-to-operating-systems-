#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for memset()
#include <unistd.h>  // for close()

// Constants
#define SERVER_ADDR "localhost"
#define SERVER_PORT 8888

int main(int argc, int *argv[]) {

    // Socket: IPV4, TCP, Default
    int socketFD = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverSocketAddr;
    
    // Create socket
    if (-1 == socketFD) {
        perror("Client failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Convert localhost into 0.0.0.0
    struct hostent *he = gethostbyname(SERVER_ADDR);
    // NBO = Network Byte Order
    unsigned long serverAddrNBO = *(unsigned long *)(he->h_addr_list[0]);


    // Configure server socket address structure (init to zero, IPv4,
    // network byte order for port and address) 
    // (Description copied from the solution code)

    memset(&serverSocketAddr, 0, sizeof(struct sockaddr_in));
    // bzero(&serverSocketAddr, sizeof(struct sockaddr_in));
    serverSocketAddr.sin_family = AF_INET;
    serverSocketAddr.sin_port = htons(SERVER_PORT); // host to net short int 16
    serverSocketAddr.sin_addr.s_addr = serverAddrNBO;


    // Connect socket to server
    if (-1 == connect(socketFD,(struct sockaddr *) &serverSocketAddr, sizeof(struct sockaddr_in))) {
        // Fail
        fprintf(stdout, "Client failed to connect to %s:%d\n", SERVER_ADDR, SERVER_PORT);
        close(socketFD);
        exit(EXIT_FAILURE);
    }
    else {
        // Success
        fprintf(stdout, "Client connected to %s:%d\n", SERVER_ADDR, SERVER_PORT);
        close(socketFD);
    }

    return 0;
}