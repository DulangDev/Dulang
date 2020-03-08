//
//  execeasy.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 08.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "execeasy.h"

#define DUL_TYPE(v) ((v).bits & 7)
#define NUM_TYPE 0
#define BOOL_TYPE 1
#define OBJ_TYPE 2



#define FETCH_VAR(c) ((c) < 0)? current->code->statics[ -(c) - 1 ] : current->start[(c)]

void exec_ctx(context * ctx){
    frame * current = ctx->current;
    int codeblock = 0;
    int max_codes_executed = ctx->current->code->op_count - (current->current_op);
    op * instr = current->code->ops + current->current_op;
    int exec_limit = INSTR_PER_ROT > max_codes_executed ? max_codes_executed : INSTR_PER_ROT;
    register value_t lhs, rhs;
    register value_t * dest;
fetch:
    if( instr - ctx->current->code->ops >=  exec_limit)
        return;
    
    void * labels [] = { &&op_plus, &&op_minus, &&op_mult, &&op_div, &&op_write};
    goto *labels[instr->opcode];
op_plus:
    lhs = FETCH_VAR(instr->second_arg);
    rhs = FETCH_VAR(instr->third_arg);
    dest = current->start + instr->first_arg;
    if( DUL_TYPE(lhs) == NUM_TYPE  && DUL_TYPE(rhs) == NUM_TYPE){
        *dest = (value_t)(lhs.value + rhs.value);
        dest->bits &= ~7;
    } else {
        *dest = (value_t)0.0;
    }
    instr++;
    goto fetch;
op_minus:
    lhs = FETCH_VAR(instr->second_arg);
    rhs = FETCH_VAR(instr->third_arg);
    dest = current->start + instr->first_arg;
    if( DUL_TYPE(lhs) == NUM_TYPE  && DUL_TYPE(rhs) == NUM_TYPE){
        *dest = (value_t)(lhs.value - rhs.value);
        dest->bits &= ~7;
    } else {
        *dest = (value_t)0.0;
    }
    instr++;
    goto fetch;
op_mult:
    lhs = FETCH_VAR(instr->second_arg);
    rhs = FETCH_VAR(instr->third_arg);
    dest = current->start + instr->first_arg;
    if( DUL_TYPE(lhs) == NUM_TYPE  && DUL_TYPE(rhs) == NUM_TYPE){
        *dest = (value_t)(lhs.value * rhs.value);
        dest->bits &= ~7;
    } else {
        *dest = (value_t)0.0;
    }
    instr++;
    goto fetch;
op_div:
    lhs = FETCH_VAR(instr->second_arg);
    rhs = FETCH_VAR(instr->third_arg);
    dest = current->start + instr->first_arg;
    if( DUL_TYPE(lhs) == NUM_TYPE  && DUL_TYPE(rhs) == NUM_TYPE){
        *dest = (value_t)(lhs.value / rhs.value);
        dest->bits &= ~7;
    } else {
        *dest = (value_t)0.0;
    }
    instr++;
    goto fetch;
op_write:
    lhs = FETCH_VAR(instr->first_arg);
    if( DUL_TYPE(lhs) == NUM_TYPE ){
        fprintf(stdout, "%lf", lhs.value);
    }
    instr++;
    goto fetch;
}
