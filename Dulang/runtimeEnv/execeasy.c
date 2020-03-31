//
//  execeasy.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 08.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "execeasy.h"

#include <unistd.h>


#define FETCH_DEST dest = current->start + instr->first_arg
#define FETCH_VAR(c) ((c) < 0)? current->code->statics[ -(c) - 1 ] : current->start[(c)]

void exec_ctx(context * ctx){
    frame * current = ctx->current;
    int codeblock = 0;
    int max_codes_executed = ctx->current->code->op_count - (current->current_op);
    op * instr = current->code->ops + current->current_op - 1;
    int exec_limit = INSTR_PER_ROT > max_codes_executed ? max_codes_executed : INSTR_PER_ROT;
    register value_t lhs, rhs;
    register value_t * dest;
    void * labels [] = { &&op_plus, &&op_minus, &&op_mult, &&op_div, &&op_write, &&op_mv};
fetch:
    instr++;
    if( instr - ctx->current->code->ops >=  exec_limit)
        return;
    lhs = FETCH_VAR(instr->second_arg);
    rhs = FETCH_VAR(instr->third_arg);
    FETCH_DEST;
    
    goto *labels[instr->opcode];
op_plus:
    
    if( DUL_TYPE(lhs) == NUM_TYPE  && DUL_TYPE(rhs) == NUM_TYPE){
        *dest = (value_t)(lhs.value + rhs.value);
        dest->bits &= ~7;
    } else {
        *dest = (value_t)0.0;
    }
    goto fetch;
op_minus:
    
    if( DUL_TYPE(lhs) == NUM_TYPE  && DUL_TYPE(rhs) == NUM_TYPE){
        *dest = (value_t)(lhs.value - rhs.value);
        dest->bits &= ~7;
    } else {
        *dest = (value_t)0.0;
    }
    goto fetch;
op_mult:
   
    if( DUL_TYPE(lhs) == NUM_TYPE  && DUL_TYPE(rhs) == NUM_TYPE){
        *dest = (value_t)(lhs.value * rhs.value);
        dest->bits &= ~7;
    } else {
        *dest = (value_t)0.0;
    }
    goto fetch;
op_div:
   
    if( DUL_TYPE(lhs) == NUM_TYPE  && DUL_TYPE(rhs) == NUM_TYPE){
        *dest = (value_t)(lhs.value / rhs.value);
        dest->bits &= ~7;
    } else {
        *dest = (value_t)0.0;
    }
    goto fetch;
op_write:
    lhs = FETCH_VAR(instr->first_arg);
    if( DUL_TYPE(lhs) == NUM_TYPE ){
        fprintf(stdout, "%lf", lhs.value);
    }
   
    goto fetch;
op_mv:
    lhs = FETCH_VAR(instr->second_arg);
    *dest = lhs;
    goto fetch;
}

void* thread_executable(void * arg){
    (void)arg;
    do{
        coro_node * n = getTask();
        exec_ctx(n->ctx);
        frame * current = n->ctx->current;
        if(current->current_op >= current->code->op_count){
            //finished
            n->ctx->current--;
            //TODO: frame erase
        } else {
            pushTask(n);
        }
    }while(!finished);
    return 0;
}

pthread_t spawn_worker(void){
    pthread_t worker;
    pthread_create(&worker, 0, thread_executable, 0);
    return worker;
}

void spawn_workers(int num){
    pthread_t * workers = (pthread_t*)malloc(sizeof(pthread_t)*num);
    pthread_t * wk = workers;
    for(int i = 0; i < num; i++){
        *wk ++ = spawn_worker();
    }
    finished = 1;
    for(int i = 0; i < num; i++){
        pthread_join(workers[i], 0);
    }
}
