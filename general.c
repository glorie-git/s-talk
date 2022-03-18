/**
* general.c --  global variables declared for sharing accross s-talk modules 
*/

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "list.h"
#include "receiver.h"
#include "input.h"
#include "transmitter.h"
#include "output.h"
#include "general.h"

int port, sin_port = 0;
struct sockaddr_in remote, local;
unsigned int len = sizeof(remote);
int s = 0; // main file descriptor
struct sockaddr_in addr;
char sin_ipaddr[20];
char sin_port_[6];                // used in getaddrinfo for peer info
struct addrinfo hints, *servinfo; // will point to getaddrinfo results
char ipstr[INET6_ADDRSTRLEN];
bool end = false;
char *buf;
bool done = false;

List *toSend, *toPrint;

pthread_t recvThread;
pthread_t sendThread;

// Thread mutexes and condition variables for modifying and reading toSend and toPrint lists
pthread_cond_t s_okToPrintCondVar = PTHREAD_COND_INITIALIZER;
pthread_cond_t s_okToSendCondVar = PTHREAD_COND_INITIALIZER;
pthread_mutex_t s_okToReadMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t s_okToModifyMutex = PTHREAD_MUTEX_INITIALIZER;

void General_init(void)
{
    toSend = List_create();
    toPrint = List_create();

    memset(&addr, 0, sizeof(addr)); // make sure the struct is empty
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port); // short, network byte order
    addr.sin_addr.s_addr = INADDR_ANY;

    // for sendto function when exiting program
    memset(&local, 0, sizeof(local)); // make sure the struct is empty
    local.sin_family = AF_INET;
    local.sin_port = htons(port); // short, network byte order
    local.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, ipstr, &(local.sin_addr));

    // make socket and check that it encountered no error
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socet");
    }

    // bind socket to network and make sure there's no error once done
    if (bind(s, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) != 0)
    {
        perror("bind");
    }

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_DGRAM;  // Datagram sockets

    // get info on peer using ip address or hostname and port number
    if (getaddrinfo(sin_ipaddr, sin_port_, &hints, &servinfo) != 0)
    {
        perror("getaddrinfo");
    }

    struct sockaddr_in *ipver;
    void *thingy;
    ipver = (struct sockaddr_in *)servinfo->ai_addr;
    thingy = &(ipver->sin_addr);

    // used for recvfrom to send messages to peer
    memset(&remote, 0, sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_port = htons(sin_port);
    remote.sin_addr.s_addr = INADDR_ANY;
    remote.sin_port = htons(sin_port);
    inet_pton(AF_INET, ipstr, &(remote.sin_addr));
    // convert the IP to a string and print it:
    inet_ntop(servinfo->ai_family, thingy, ipstr, sizeof ipstr);
    printf("%s\n", ipstr);

    Receiver_init();
    Input_init();
    Transmitter_init();
    Output_init();
}

void General_shutdown()
{
    Input_shutdown();
    Output_shutdown();
    Receiver_shutdown();
    Transmitter_shutdown();

    freeaddrinfo(servinfo);
    close(s);

    // free(msgRx);
    // free(msgTx);

    printf("chat ended...\n");
}