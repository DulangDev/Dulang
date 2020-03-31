//
//  coro_sheduler.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 03.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "coro_sheduler.h"

coro_node * new_coro(context * ctx){
    coro_node * n = (coro_node*)malloc(sizeof(coro_node));
    n->ctx = ctx;
    n->next = n->prev = 0;
    return n;
}

struct _coronode * getTask(void){
    pthread_mutex_lock(&coro_mut);
    coro_node * c = coro_head;
    coro_head = coro_head ? coro_head->next : 0;
    pthread_mutex_unlock(&coro_mut);
    return c;
}
void pushTask  (struct _coronode* c){
    pthread_mutex_lock(&coro_mut);
    coro_node ** walker = &coro_head;
    if(*walker){
        walker = &((*walker)->next);
    }
    *walker = c;
    pthread_mutex_unlock(&coro_mut);
}
