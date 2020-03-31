//
//  coro_sheduler.h
//  Dulang
//
//  Created by Дмитрий Маслюков on 03.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef coro_sheduler_h
#define coro_sheduler_h
#include <pthread.h>
#include "frame.h"
//implements a queue
typedef struct _coronode{
    context * ctx;
    struct _coronode * next;
    struct _coronode * prev;
} coro_node;

static coro_node * coro_head;
static pthread_mutex_t coro_mut = PTHREAD_MUTEX_INITIALIZER;

struct _coronode * getTask(void);
void pushTask  (struct _coronode*);
coro_node * new_coro(context * ctx);

#endif /* coro_sheduler_h */
