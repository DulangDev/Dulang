//
//  frame.h
//  Dulang
//
//  Created by Дмитрий Маслюков on 03.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef frame_h
#define frame_h

#include "../compiler/bytecodegen.h"
#define INIT_MEM_SIZE 1024

//TODO: replace internal code with codeobject that holds closures
typedef struct {
    internal_code * code;
    value_t * start;
    int current_op;
} frame;

typedef struct {
    char is_waiting;
    frame frames [1024];
    value_t * memory;
    size_t memory_cap;
    frame * current;
    
} context;
context* init_context(void);
void start_frame(context * ctx, internal_code * code);
void unlink_frame(context * ctx);
#endif /* frame_h */
