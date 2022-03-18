/**
* receiver.c --  module handles receiving messages from peer and modifies condition variables & mutexes
*/

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "list.h"
#include "receiver.h"
#include "general.h"

extern struct addrinfo *servinfo;
extern int port, sin_port;
extern struct sockaddr_in remote;
extern unsigned int len;
extern int s;
extern struct sockaddr_in addr;
extern bool end, done;
extern char *buf;

extern List *toSend, *toPrint;

extern pthread_cond_t s_okToPrintCondVar;
extern pthread_cond_t s_okToSendCondVar;
extern pthread_mutex_t s_okToReadMutex;
extern pthread_mutex_t s_okToModifyMutex;

extern pthread_t recvThread, sendThread;

void *recvThreadFunc(void *unsued)
{
    char *msgRx = (char *)malloc(MAX_MSG_SIZE);
    strcpy(msgRx, "");
    while (strcmp(msgRx, "!\n"))
    {
        if (recvfrom(s, msgRx, MAX_MSG_SIZE, 0, (struct sockaddr *)&remote, &len) == -1)
        {
            perror("recvfrom");
            exit(1);
        }

        if (done) // verify that no threads have exited
        {
            break;
        }
        pthread_mutex_lock(&s_okToModifyMutex);
        {
            List_add(toPrint, msgRx);
            pthread_cond_signal(&s_okToPrintCondVar);
        }
        pthread_mutex_unlock(&s_okToModifyMutex);
        if (done)
        {
            break;
        }
    }
    end = true;
    done = true;
    strcpy(buf, "!\n");
    pthread_cond_signal(&s_okToPrintCondVar);
    free(msgRx);
    return NULL;
}

void Receiver_init(void)
{
    pthread_create(&recvThread, NULL, recvThreadFunc, NULL);
}

void Receiver_shutdown(void)
{
    // printf("sb1--re\n");
    pthread_join(recvThread, NULL);
    // printf("sb2--re\n");
    pthread_cancel(recvThread);
    // printf("sb3--re\n");
}
