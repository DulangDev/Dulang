//
//  bytecodegen.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 13.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "bytecodegen.h"
#include <string.h>

#define STAT_MASK 0xfffffffffffffff8
#define MAX(x, y) (x) > (y)? (x):(y);

internal_code new_code(void){
    internal_code code;
    code.name_count = 0;
    code.name_cap = 15;
    code.names = (char**)malloc(sizeof(char*)*15);
    code.op_cap = 1000;
    code.op_count = 0;
    code.ops = (op*)malloc(1000*sizeof(op));
    code.static_count = 0;
    code.static_cap = 15;
    code.statics = (value_t*)malloc(sizeof(value_t)*15);
    return code;
}

static double num_from_ptr(void * ptr){
    long a =  (*(long*)&ptr) >> 3;
    return *(double*) (&a);
}

static op* add_op(internal_code * code, char opcode, reg_t dest, reg_t first, reg_t second){
    if( code->op_count ++ == code->op_cap ){
        code->op_cap *= 2;
        code->ops = (op*)realloc(code->ops, (sizeof(op)*code->op_cap));
    }
    op new_op =  {opcode, dest, first, second};
    code->ops[ code->op_count - 1 ] = new_op;
    return code->ops + (code->op_count - 1);
}

static reg_t first_reg(const internal_code * code){
    return code->name_count;
}

static void add_name(internal_code * code, const char * name){
    for(int i = 0; i < code->name_count; i++){
        if( strcmp(name, code->names[i]) == 0 ){
            return;
        }
    }
    //not found
    if( code->name_count++ == code->name_cap ){
        code->name_cap <<= 1;
        code->names = (char**)realloc(code->names, sizeof(char*)*code->name_cap);
    }
    code->names[code->name_count - 1] = strdup(name);
}

static int add_static(internal_code * code, astnode * node){
    //here it is transformed into dulang object
    for(int i = 0; i < node->childrencount; i++){
        add_static(code, node->children[i]);
    }
    
    //TODO: for other literals
    if( node->type == NUMLIT ){
        if( code->static_count++ == code->static_cap ){
            code->static_cap <<= 1;
            code->statics = (value_t*)realloc(code->statics, sizeof(value_t)*code->static_cap);
        }
        code->statics[code->static_count - 1].bits = (*(long*)&node->val) & STAT_MASK + 1;
        return code->static_count;
    }
    return 0;
}

static void process_names(internal_code * code, astnode * node){
    for(int i = 0; i < node->childrencount; i++){
        process_names(code, node->children[i]);
    }
    if(node->type == NAME){
        add_name(code, (const char*)node->val);
    }
}

static reg_t name_lookup(internal_code * code, const char * name){
    for(int i = 0; i < code->name_count; i++){
        if( strcmp(name, code->names[i]) == 0 ){
            return i;
        }
    }
    return 0;
}

void load_module(internal_code * code, astnode * root){
    code->total_reg_max = root->registers_needed;
    add_name(code, "this");
    process_names(code, root);
    load_node(code, root, 0);
}

static void load_reg(internal_code *code, astnode *lhs, reg_t *lhs_reg) {
    switch (lhs->type) {
        case NUMLIT:
            *lhs_reg = - add_static(code, lhs);
            break;
        case NAME:
            *lhs_reg = name_lookup(code, (const char*)lhs->val);
            break;
        default:
            load_node(code, lhs, first_reg(code));
            *lhs_reg = code->curr_reg_max;
            break;
    }
}

reg_t load_node(internal_code * code, astnode * node, reg_t dest){
    if(node->type == COMPOUND){
        for(int i = 0; i < node->childrencount; i++){
            code->curr_reg_max = first_reg(code);
            load_node(code, node->children[i], first_reg(code));
        }
        return 0;
    }
    
    if(node->type == ASSIGN){
        reg_t destination = first_reg(code);
        //TODO: for other types of assign
        if( node->children[0]->type == NAME ){
            destination = name_lookup(code, (char*)node->children[0]->val);
        }
        load_node(code, node->children[1], destination);
        return 0;
    }
    
    
    //nodes with predefined positions
    if(node->type == NUMLIT){
        return - add_static(code, node);
    }
    
    if(node->type == NAME){
        return name_lookup(code, (char*)node->val);
    }
    //standard binary arithmetics, can be treated same way
    if(node->type == PLUS || node->type == MINUS || node->type == MULT || node->type == DIV){
        astnode * lhs = node->children[0];
        astnode * rhs = node->children[1];
        reg_t lhs_reg = code->curr_reg_max, rhs_reg = code->curr_reg_max + 1;
        lhs_reg = load_node(code, lhs, lhs_reg);
        rhs_reg = load_node(code, rhs, rhs_reg);
        code->curr_reg_max = MAX(code->curr_reg_max, lhs_reg);
        code->curr_reg_max = MAX(code->curr_reg_max, rhs_reg);
        char opcode;
        switch (node->type) {
            case MINUS:
                opcode = minus_op;
                break;
            case MULT:
                opcode = mult_op;
                break;
            case DIV:
                opcode = div_op;
                break;
            default:
                opcode = plus_op;
                break;
        }
        add_op(code, opcode, dest, lhs_reg, rhs_reg);
        return dest;
    }
    
    
    
    if(node->type == WRITE){
        reg_t dest = first_reg(code);
        reg_t output = load_node(code, node->children[0], dest);
        add_op(code, write_op, output, 0, 0);
    }
    
    return 0;
}

static void print_indents(FILE * output, int amount){
    for(int i = 0; i < amount; i++){
        fputc(' ', output);
    }
}

void print_code(internal_code * code, FILE * output, int indentation){
    print_indents(output, indentation);
    fprintf(output, "place needed: %d\n", code->name_count + code->total_reg_max);
    print_indents(output, indentation);
    fprintf(output, "names: \n");
    for(int i = 0; i < code->name_count; i++){
        print_indents(output, indentation + 4);
        fprintf(output, "%.20s : %d\n", code->names[i], i);
    }
    print_indents(output, indentation);
    fprintf(output, "static values:\n");
    //TODO: for other static types, e.g. strings and codes
    for(int i = 0; i < code->static_count; i++){
        print_indents(output, indentation + 4);
        value_t value = code->statics[i];
        value.bits &= STAT_MASK;
        fprintf(output, "%lf : %d\n", value.value , -i - 1);
    }
    print_indents(output, indentation);
    fprintf(output, "code:\n");
    for(int i = 0; i < code->op_count; i++){
        print_indents(output, indentation + 4);
        op * current = &code->ops[i];
        fprintf(output, "%d %s : %.3d : %.3d : %.3d\n", i,  opcode_repr[current->opcode] ,current->first_arg, current->second_arg, current->third_arg);
    }
}
