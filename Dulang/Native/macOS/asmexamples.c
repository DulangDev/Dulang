//
//  asmexamples.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 01.04.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "asmexamples.h"
#include <string.h>
#include <errno.h>
#include "../../runtimeEnv/func_wrappers.h"
static void write_mem(asmwriter*writer, const void *mem, size_t memsize){
    size_t demand = writer->mem_size + memsize;
    if( writer->memcap <= demand ){
        writer->memcap = demand * 1.5;
        writer->mem = realloc(writer->mem, writer->memcap);
    }
    memcpy(writer->mem + writer->mem_size, mem, memsize);
    writer->mem_size += memsize;
}
static void write_headers(asmwriter * writer){
    unsigned char mem [] = {
        0x55, //pushq %rbp
        0x48, 0x89, 0xe5, // movq %rsp, %rbp
        0x48, 0x83, 0xec, 0x10, // subq $16, %rsp (почему-то я считаю, что четвертый байт это второй аргумент)
        0x48, 0x89, 0x7d, 0xf8,  // movq    %rdi, -8(%rbp)
        0x48, 0x89, 0x75, 0xf0,  // movq    %rsi, -16(%rbp)
    };
    
    write_mem(writer, mem, sizeof(mem));
}

static void write_footer(asmwriter *writer){
    unsigned char mem[] = {
        0xc3, 0x5d
    };
    write_mem(writer, mem, 1);
}

asmwriter create_writer(void){
    asmwriter writer;
    writer.mem = malloc(1024);
    writer.memcap = 1024;
    writer.mem_size = 0;
    write_headers(&writer);
    return writer;
}

#define write_internal_array(a) write_mem(writer, a, sizeof(a))

void write_vm_instr(asmwriter*writer, DulIROP op){
    char mem_write [1024];
    switch (op.opcode) {
        case istore:{
            unsigned char intsr1 [] = {
                0x48, 0x8b, 0x75, 0xf8, // movq -8(%rbp), %rsi
                0x48, 0x8b, 0x76, (char)op.first_arg  // movq (place-in-static)(%rsi), %rsi
            };
            write_internal_array(intsr1);
            unsigned char instr2 [] = {
                0x48, 0x8b, 0x7d, 0xf0,    //movq    -16(%rbp), %rdi
                0x48, 0x89, 0x77, (char)op.dest_arg     //movq    %rsi, 24(%rdi)
            };
            write_internal_array(instr2);
        }break;
        case iwrite_:{
            unsigned char address [8];
            memcpy(address, print_integer, 8);
            unsigned char inst [] = {
                0x48, 0x8b, 0x75, 0xf0,   // movq    -16(%rbp), %rsi
                0x48, 0x83, 0xc6, (char)op.dest_arg,  //addq    (place-in-stack), %rsi
                0x48, 0x8b
            };
            write_internal_array(inst);
            write_internal_array(address);
            unsigned char inst_footer [] = {
                0xff, 0xd0 //call %rax
            };
            write_internal_array(inst_footer);
        }break;
        default:
            break;
    }
}


asmfunc generate_from_writer(const asmwriter writer){
    write_footer(&writer);
    void * mem = mmap(0, writer.mem_size,   PROT_READ|PROT_WRITE , MAP_ANONYMOUS| MAP_PRIVATE  , -1, 0);
    if(mem == MAP_FAILED){
        perror("mmap failed");
    }
    //
    
    
    for(char * rdr = writer.mem, *wr = mem; rdr < (char*)writer.mem + writer.mem_size; rdr++, wr++ ){
        *wr = *rdr;
    }
    int status = mprotect(mem, writer.mem_size, PROT_EXEC|PROT_READ);
    if(status != 0){
        perror("mprotect");
    }
    return (asmfunc)mem;
}


void write_vm_code(internal_code*c, asmwriter*writer){
    for(int i = 0; i < c->op_count; i++){
        write_vm_instr(writer, c->ops[i]);
    }
}
