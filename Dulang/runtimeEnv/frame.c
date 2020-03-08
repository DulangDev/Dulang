//
//  frame.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 03.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "frame.h"

context* init_context(void){
    context * ctx = (context*)malloc(sizeof(context));
    ctx->current = ctx->frames;
    ctx->memory = (value_t*)malloc(sizeof(value_t)*INIT_MEM_SIZE);
    ctx->memory_cap = INIT_MEM_SIZE;
    return ctx;
}

void start_frame(context * ctx, internal_code * code){
    
    value_t * new_start = ctx->memory;
    if(ctx->current != ctx->frames)
    new_start = ctx->current->start + ctx->current->code->total_reg_max;
    while(ctx->memory_cap < (new_start - ctx->memory) + code->total_reg_max){
        //need to realloc
        ctx->memory_cap *= 2;
        ctx->memory = (value_t*)realloc(ctx->memory, sizeof(value_t)*ctx->memory_cap);
    }
    frame fr = {
        code,
        new_start,
        0
    };
    *ctx->current = fr;
}
