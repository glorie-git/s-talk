/**
* output.c --  module handles printing messages from peer to screen
*/

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "list.h"
#include "general.h"

extern List *toSend, *toPrint;
extern bool end, done;
extern char *buf;

extern pthread_cond_t s_okToPrintCondVar;
extern pthread_cond_t s_okToSendCondVar;
extern pthread_mutex_t s_okToReadMutex;
extern pthread_mutex_t s_okToModifyMutex;

// Does nothing other than await input from the keyboard.
void *outputThreadFunc(void *unsued)
{
    end = false;
    while (1)
    {
        // printf("begin --ot\n");
        pthread_cond_wait(&s_okToPrintCondVar, &s_okToModifyMutex);
        // printf("1\n");
        if (done)
        {
            break;
        }
        pthread_mutex_lock(&s_okToReadMutex);
        {
            fflush(stdout);
            // printf("stuck\n");
            fputs("> ", stdout);
            fputs(toPrint->current->item, stdout);
            // printf("2\n");
        }
        pthread_mutex_unlock(&s_okToReadMutex);
        // printf("3\n");
        if (done)
        {
            // printf("end program*****\n");
            break;
        }
    }
    done = true;
    // strcpy(buf, "!\n");
    // printf("**out**");
    return NULL;
}

pthread_t outputThread;

void Output_init(void)
{
    pthread_create(&outputThread, NULL, outputThreadFunc, NULL);
}

void Output_shutdown(void)
{
    // pthread_cond_signal(&s_okToPrintCondVar);
    // pthread_mutex_unlock(&s_okToReadMutex);
    // printf("sb1--ou\n");
    pthread_join(outputThread, NULL);
    // printf("sb2--ou\n");
    pthread_cancel(outputThread);
    // printf("sb3--ou\n");
}