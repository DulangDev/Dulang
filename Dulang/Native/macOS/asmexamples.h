//
//  asmexamples.h
//  Dulang
//
//  Created by Дмитрий Маслюков on 01.04.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef asmexamples_h
#define asmexamples_h

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "../../compiler/bytecodegen.h"
typedef struct {
    void * mem;
    size_t mem_size;
    size_t memcap;
} asmwriter;

asmwriter create_writer(void);

void write_vm_instr(asmwriter*writer, DulIROP op);
void write_vm_code(internal_code*c, asmwriter*writer);

typedef void * (*asmfunc)(void* staticptr, void* stackptr);

asmfunc generate_from_writer(const asmwriter writer);

#endif /* asmexamples_h */
