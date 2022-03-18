#ifndef _GENERAL_H_
#define _GENERAL_H_
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <pthread.h>
#define MAX_MSG_SIZE 256
#endif

void General_init(void);

void General_shutdown(void);