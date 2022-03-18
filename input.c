/**
* input.c --    module handles reading user input and 
                modifies the list toSend to que in messages to be sent peer
*/

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/select.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "list.h"
#include "general.h"

extern List *toSend, *toPrint;

extern pthread_cond_t s_okToPrintCondVar;
extern pthread_cond_t s_okToSendCondVar;
extern pthread_mutex_t s_okToReadMutex;
extern pthread_mutex_t s_okToModifyMutex;
extern bool end, done;
extern char *buf;

struct sockaddr_in local;
unsigned int len;

// Does nothing other than await input from the keyboard.
// uses select() to refrech stdin read
void *inputThreadFunc(void *unsued)
{
    // char *msgTx = (char *)malloc(MAX_MSG_SIZE);
    // strcpy(msgTx, "null");
    int fd;
    buf = malloc(MAX_MSG_SIZE);
    int ret, sret;

    fd = 0;

    fd_set readfds;
    struct timeval timeout;

    while (1)
    {

        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);

        // fresh reading user input every 2 seconds
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

        // create file descriptor to monitor
        // this file descrripter monites the stdin
        sret = select(8, &readfds, NULL, NULL, &timeout);

        if (sret == 0)
        {
            // done has been set to done it means program needs to end
            // prison break from this loop!
            if (done)
            {
                break;
            }
        }
        else
        {
            memset((void *)buf, 0, MAX_MSG_SIZE);
            ret = read(fd, (void *)buf, MAX_MSG_SIZE);

            if (ret != -1)
            {
                pthread_mutex_lock(&s_okToReadMutex);
                {
                    List_add(toSend, buf);
                    pthread_cond_signal(&s_okToSendCondVar);
                }
                pthread_mutex_unlock(&s_okToReadMutex);

                // another to conditions to help break from the loop
                if (done)
                {
                    break;
                }
                if (!strcmp(buf, "!\n"))
                {
                    break;
                }
            }
        }
    }

    // printf("**input--done**\n");
    pthread_cond_signal(&s_okToSendCondVar);
    done = true;
    end = true;
    free(buf);
    // free(msgTx);
    return NULL;
}

pthread_t inputThread;

void Input_init(void)
{
    pthread_create(&inputThread, NULL, inputThreadFunc, NULL);
}

void Input_shutdown(void)
{
    pthread_join(inputThread, NULL);
    pthread_cancel(inputThread);
}