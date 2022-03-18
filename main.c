/**
*   main.c --
*/

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "list.h"
#include "general.h"

// Global variables
extern int port, sin_port;
extern char sin_ipaddr[20], sin_port_[6], ipstr[INET6_ADDRSTRLEN];

int main(int argc, char **args)
{
    if (argc == 4)
    {
        port = atoi(args[1]);
        stpcpy(sin_ipaddr, args[2]);
        stpcpy(sin_port_, args[3]); // used in getaddrinfo for peer info
        sin_port = atoi(args[3]);

        General_init();

        printf("MY PORT: %s || PEER PORT: %s & IP ADDRESS: %s\n", args[1], args[3], ipstr);
        printf("chat started...\n\n");

        General_shutdown();
    }
    else
    {
        printf("error: verify arguments\n");
        printf("s-talk [my port number] [remote machine name] [remote port number]\n");
    }

    return 0;
}
