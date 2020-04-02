//
//  byctoasm.h
//  Dulang
//
//  Created by Дмитрий Маслюков on 30.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef byctoasm_h
#define byctoasm_h

#include "../../compiler/bytecodegen.h"
enum asm_opcodes{
    divsd,
    movq,
    movsd,
    mulsd,
    addq,
    subq,
    mulq,
    divq
};

typedef struct {
    int16_t opc;
    //null-terminated
    char args [16];
} asm_instruction;

typedef struct{
    asm_instruction * instructions;
    size_t instr_cap;
    size_t instr_count;
} asm_bytearray;

asm_bytearray create_asm_array(void);
void add_instruction(asm_bytearray*, asm_instruction);
void generate_instructions(asm_bytearray*, DulIROP);

#endif /* byctoasm_h */
