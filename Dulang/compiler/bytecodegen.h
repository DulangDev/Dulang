//
//  bytecodegen.h
//  Dulang
//
//  Created by Дмитрий Маслюков on 13.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef bytecodegen_h
#define bytecodegen_h

#include "astgen.h"
#include "../objects/object.h"
#include <stdint.h>
typedef int16_t reg_t;
enum opcodes{
    plus_op,
    minus_op,
    mult_op,
    div_op,
    write_op
    
};

static const char * opcode_repr [] = {
    "plus",
    "minus",
    "mult",
    "div",
    "assign",
    "write"
};

typedef struct {
    char opcode;
    reg_t first_arg;
    reg_t second_arg;
    reg_t third_arg;
    void * inline_cache [3];
} op;



typedef struct {
    int name_count;
    int name_cap;
    char ** names;
    int op_count;
    int op_cap;
    op * ops;
    int static_count;
    int static_cap;
    value_t* statics;
    //sets to 0 after each statement
    int curr_reg_max;
    int total_reg_max;
} internal_code;

internal_code new_code(void);
void load_module(internal_code* code, astnode * root);
reg_t load_node(internal_code * code, astnode * root, reg_t dest);
void print_code(internal_code * code, FILE * output, int);

#endif /* bytecodegen_h */
