/**
* transmitter.c --  module handles sending messages to peer 
                    and modifies condition variables & mutexes
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
extern int s;
extern char *buf;
extern bool end;

extern struct sockaddr_in remote, local;
extern unsigned int len;
extern bool done;

extern List *toSend, *toPrint;

extern pthread_cond_t s_okToPrintCondVar;
extern pthread_cond_t s_okToSendCondVar;
extern pthread_mutex_t s_okToReadMutex;
extern pthread_mutex_t s_okToModifyMutex;

extern pthread_t sendThread, recvThread;

// A thread which sends data to the remote UNIX process over the network using UDP.
void *sendThreadFunc(void *unsued)
{
    char *msgTx = (char *)malloc(MAX_MSG_SIZE);
    strcpy(msgTx, "");

    while (strcmp(msgTx, "!\n"))
    {
        pthread_cond_wait(&s_okToSendCondVar, &s_okToReadMutex);
        if (done)
        {
            if (sendto(s, "!\n", MAX_MSG_SIZE, 0, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
            {
                perror("sendto before break");
            }
            break;
        }
        pthread_mutex_lock(&s_okToModifyMutex);
        {
            stpcpy(msgTx, toSend->current->item);
        }
        pthread_mutex_unlock(&s_okToModifyMutex);
        if (sendto(s, msgTx, MAX_MSG_SIZE, 0, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
        {
            perror("sendto");
        }
        if (done)
        {
            break;
        }
    }
    if (sendto(s, "!\n", MAX_MSG_SIZE, 0, (struct sockaddr *)&local, sizeof(struct sockaddr_in)) == -1)
    {
        perror("sendto");
    }
    free(msgTx);
    end = true;
    done = true;
    pthread_mutex_unlock(&s_okToModifyMutex);
    pthread_cond_signal(&s_okToPrintCondVar);
    return NULL;
}

void Transmitter_init(void)
{
    pthread_create(&sendThread, NULL, sendThreadFunc, NULL);
}

void Transmitter_shutdown(void)
{
    pthread_join(sendThread, NULL);
    pthread_cancel(sendThread);
}
