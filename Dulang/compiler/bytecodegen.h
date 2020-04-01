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
#include "optimizer.h"
#include "../objects/type.h"
#include <stdint.h>
typedef int16_t reg_t;
// m suffix stands for immediate value
// lm means left operand is immediate, rm means right is
// for single operand like write it is only with or without m suffix
enum opcodes{
    iplus,
    fplus,
    ipluslm,
    iplusrm,
    fpluslm,
    fplusrm,
    iminus,
    fminus,
    iminuslm,
    iminusrm,
    fminuslm,
    fminusrm,
    imult,
    fmult,
    imultlm,
    imultrm,
    fmultlm,
    fmultrm,
    idiv,
    fdiv,
    idivlm,
    idivrm,
    fdivlm,
    fdivrm,
    iwrite_,
    iwrite_m,
    fwrite_,
    fwrite_m,
    istore,
    fstore,
    cast_to_float,
    cast_to_float_m
};

static const char * opcode_repr [] = {
    "integer add",
    "float add",
    "integer add lm",
    "integer add rm",
    "float add lm",
    "float add rm",
    "integer sub",
    "float sub",
    "integer sub lm",
    "integer sub rm",
    "float sub lm",
    "float sub rm",
    "integer mult",
    "float mult",
    "integer mult lm",
    "integer mult rm",
    "float mult lm",
    "float mult rm",
    "integer div",
    "float div",
    "integer div lm",
    "integer div rm",
    "float div lm",
    "float div rm",
    "integer write",
    "integer write m",
    "float write",
    "float write m",
    "store integer",
    "store float",
    "cast to float",
    "cast to float m"
};

typedef struct {
    reg_t opcode;
    long dest_arg;
    long first_arg;
    long second_arg;
} DulVMOP;



typedef struct {
    dul_layout_type * layout;
    int op_count;
    int op_cap;
    DulVMOP * ops;
    void * static_pos;
    int static_cap;
    void * statics;
    //sets to 0 after each statement
    size_t current_stack_offset;
    size_t total_stack_space_need;
} internal_code;

//needs nametable to initialize
internal_code new_code(astnode * root);
void load_node(internal_code * code, astnode * node);
internal_code generate_code_from_source(const char * fname);
void print_code(FILE*fout, internal_code*c);

#endif /* bytecodegen_h */
