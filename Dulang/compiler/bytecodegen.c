//
//  bytecodegen.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 13.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "bytecodegen.h"
#include <string.h>

internal_code new_code(astnode * root){
    dul_layout_type * name_table = root->name_table;
    internal_code code;
    code.layout = name_table;
    code.op_cap = 100;
    code.ops = (DulIROP*)malloc(sizeof(DulIROP)*100);
    code.op_count = 0;
    //bytes
    code.static_cap = 1024;
    code.statics = malloc(1024);
    code.static_pos = code.statics;
    code.current_stack_offset = root->name_table->size;
    return code;
}

static void write_op(internal_code * c, DulIROP operand){
    if(c->op_count ++ == c->op_cap){
        c->op_cap += 100;
        c->ops = realloc(c->ops, (c->op_cap)*sizeof(DulIROP));
    }
    c->ops[c->op_count - 1] = operand;
}


static size_t add_static(internal_code*code, void * static_value, size_t size){
    if( code->static_pos - code->statics + size < code->static_cap ){
        code->static_cap += 1024;
        size_t stat_offset = code->static_pos - code->statics;
        code->statics = realloc(code->statics, code->static_cap);
        code->static_pos = code->statics + stat_offset;
    }
    memcpy(code->static_pos, static_value, size);
    code->static_pos += size;
    return code->static_pos - code->statics - size;
}

enum math_specs {
    iop = 0,
    fop = 1,
    ioplm = 2,
    ioprm = 3,
    foplm = 4,
    foprm = 5
};

enum math_specs configure_spec(astnode * node){
    //assume node has type of PLUS, MINUS, MULT, DIV
    int is_integer = node->node_type->typeid == INT_ID;
    if(is_integer){
        //both are integer
        if(node->children[0]->type == INTEGERLIT)
            return ioplm;
        if(node->children[1]->type == INTEGERLIT)
            return ioprm;
        return iop;
    }else{
        //both of operands are float, maybe casted
        if(node->children[0]->type == NUMLIT)
            return foplm;
        if(node->children[1]->type == NUMLIT)
            return foprm;
        return fop;
    }
    return iop;
}



static long dest_preview_load(internal_code *c, astnode * node, long dest){
    switch (node->type) {
        case MINUS:
        case MULT:
        case DIV:
        case PLUS:{
            long lhs = dest_preview_load(c, node->children[0], -1);
            long rhs = dest_preview_load(c, node->children[1], -1);
            enum opcodes type = iplus + (iminus - iplus)*(node->type - PLUS);
            //lets configure type spec
            size_t real_dest = 0;
            if(dest == -1){
                real_dest = c->current_stack_offset;
                c->current_stack_offset += 8;
            } else {
                real_dest = dest;
            }
            type += configure_spec(node);
            DulIROP operation = {
                type,
                real_dest,
                lhs,
                rhs
            };
            write_op(c, operation);
            return real_dest;
        }break;
        case INTEGERLIT:{
            return add_static(c, node->val, sizeof(int64_t));
        }break;
        case NUMLIT:{
            return add_static(c, node->val, sizeof(double));
        }break;
        case NAME:{
            return layout_count_offset(node->name_table, (char*)node->val);
        };
        case CAST_TO_FLOAT:{
            enum opcodes type = cast_to_float;
            if(node->children[0]->type == INTEGERLIT){
                type = cast_to_float_m;
            }
            size_t arg_place = dest_preview_load(c, node->children[0], -1);
            size_t real_dest = 0;
            if(dest == -1){
                real_dest = c->current_stack_offset;
                c->current_stack_offset += 8;
            } else {
                real_dest = dest;
            }
            DulIROP operation = {
                type,
                real_dest,
                arg_place,
                0
            };
            write_op(c, operation);
            return real_dest;
        }break;
        case WRITE:{
            size_t arg_place = dest_preview_load(c, node->children[0], -1);
            enum opcodes write_t = iwrite_;
            if(node->children[0]->type == INTEGERLIT){
                write_t = iwrite_m;
            }
            if(node->children[0]->node_type->typeid == FLOAT_ID){
                write_t = fwrite_;
            }
            if(node->children[0]->type == NUMLIT){
                write_t = fwrite_m;
            }
            DulIROP operation = {
                write_t,
                arg_place
            };
            write_op(c, operation);
            return -1;
        }break;
        case ASSIGN:{
            //TODO: generate dest for subscr
            size_t rdest = layout_count_offset(node->name_table, (char*)node->children[0]->val);
            if(node->children[1]->type == NUMLIT){
                //assume that types are properly casted
                size_t arg_place = add_static(c, node->children[1]->val, sizeof(double));
                DulIROP operation = {
                    fstore,
                    rdest,
                    arg_place
                };
                write_op(c, operation);
                return -1;
            }
            if(node->children[1]->type == INTEGERLIT){
                size_t arg_place = add_static(c, node->children[1]->val, sizeof(int64_t));
                DulIROP operation = {
                    istore,
                    rdest,
                    arg_place,
                    0
                };
                write_op(c, operation);
                return -1;
            }
            dest_preview_load(c, node->children[1], rdest);
            return -1;
        }break;
        default:
            break;
    }
    return 0l;
}

void load_node(internal_code * code, astnode * node){
    
    switch (node->type) {
        case COMPOUND:{
            for(int i = 0; i < node->childrencount; i++){
                
                dest_preview_load(code, node->children[i], -1);
            }
        }break;
            
        default:
            break;
    }
}

static int is_arithmetic(enum opcodes opc){
    return opc <= fdivrm;
}
#define IS_LM 1
#define IS_RM 2
#define NOT_M 0
static int imm_type(enum opcodes opc){
    int spec = opc % 6;
    switch (spec) {
        case 0:
        case 1:
            return NOT_M;
            break;
        case 2:
        case 4:
            return IS_LM;
            break;
        case 3:
        case 5:
            return IS_RM;
            break;
        default:
            break;
    }
    return NOT_M;
}
#define INT_FAMILY 0
#define FLOAT_FAMILY 1
static int get_type_family(enum opcodes opc){
    int spec = opc % 6;
    switch (spec) {
        case 0:
        case 2:
        case 3:
            return INT_FAMILY;
        default:
            break;
    }
    return FLOAT_FAMILY;
}

static int imm_misc_type(enum opcodes opc){
    return opcode_repr[opc][strlen(opcode_repr[opc]) - 1] == 'm';
}

static void print_not_m_dest(char * buf, internal_code * c, size_t dest){
    if(dest >= c->layout->size){
        sprintf(buf, "(stack+%lu)", dest - c->layout->size);
    } else {
        sprintf(buf, "(var:%s)", get_var_at_layout(c->layout, dest));
    }
}

static void print_operation(FILE*fout, internal_code*c, int index){
    DulIROP opc = c->ops[index];
    fprintf(fout, "%d: %s ", index, opcode_repr[opc.opcode]);
    if(is_arithmetic(opc.opcode)){
        int immtype = imm_type(opc.opcode);
        int type_family = get_type_family(opc.opcode);
        size_t dest = opc.dest_arg;
        char dest_repr [32];
        print_not_m_dest(dest_repr, c, dest);
        fprintf(fout, "%s <- ", dest_repr);
        if(immtype){
            if(type_family == INT_FAMILY){
                if(immtype == IS_LM){
                    print_not_m_dest(dest_repr, c, opc.second_arg);
                    fprintf(fout, "%lld, %s\n", *(int64_t*)(c->statics + opc.first_arg), dest_repr);
                    return;
                } else {
                    print_not_m_dest(dest_repr, c, opc.first_arg);
                    fprintf(fout, "%s, %lld\n", dest_repr,  *(int64_t*)(c->statics + opc.second_arg));
                    return;
                }
            } else {
                if(immtype == IS_LM){
                    print_not_m_dest(dest_repr, c, opc.second_arg);
                    fprintf(fout, "%lf, %s\n", *(double*)(c->statics + opc.first_arg), dest_repr);
                    return;
                } else {
                    print_not_m_dest(dest_repr, c, opc.first_arg);
                    fprintf(fout, "%s, %lf\n", dest_repr,  *(double*)(c->statics + opc.second_arg));
                    return;
                }
            }
        } else {
            char first_repr [32], sec_repr[32];
            print_not_m_dest(first_repr, c, opc.first_arg);
            print_not_m_dest(sec_repr, c, opc.second_arg);
            fprintf(fout, "%s, %s\n", first_repr, sec_repr);
            return;
        }
    } else if(opc.opcode == cast_to_float || opc.opcode == cast_to_float_m){
        char dest_repr [32];
        print_not_m_dest(dest_repr, c, opc.dest_arg);
        if(opc.opcode == cast_to_float_m){
            fprintf(fout, "%s <- %lld\n", dest_repr, *(int64_t*)opc.first_arg);
        }else{
            char arg_repr [32];
            print_not_m_dest(arg_repr, c, opc.first_arg);
            fprintf(fout, "%s <- %s\n", dest_repr, arg_repr);
        }
    } else if(opc.opcode == iwrite_|| opc.opcode == fwrite_ ){
        char arg_repr [32];
        print_not_m_dest(arg_repr, c, opc.dest_arg);
        fprintf(fout, "%s\n", arg_repr);
    } else if(opc.opcode == iwrite_m){
        fprintf(fout, "%lld\n", *(int64_t*)(c->statics +opc.dest_arg));
    } else if(opc.opcode == fwrite_m){
        fprintf(fout, "%lf\n", *(double*)(c->statics + opc.dest_arg));
    } else if(opc.opcode == istore){
        char dest_buf [32];
        print_not_m_dest(dest_buf, c, opc.dest_arg);
        fprintf(fout, "%s <- %lld\n", dest_buf, *(int64_t*)((char*)c->statics+ opc.first_arg));
    } else if(opc.opcode == fstore){
        char dest_buf [32];
        print_not_m_dest(dest_buf, c, opc.dest_arg);
        fprintf(fout, "%s <- %lf\n", dest_buf, *(double*)(c->statics + opc.first_arg));
    }
    else fprintf(fout, "%3ld %3ld %3ld\n", opc.dest_arg, opc.first_arg, opc.second_arg);
}
void print_code(FILE*fout, internal_code*c){
    for(int i = 0; i < c->op_count; i++){
        print_operation(fout, c, i);
    }
}

internal_code generate_code_from_source(const char * fname){
    astnode * root = astparse_file(fname);
    char buf [1000];
    sprintf(buf, "%s.ast", fname);
    FILE * f = fopen(buf, "w");
    print_astnode(root, f, 0);
    fclose(f);
    sprintf(buf, "%s.dulcode", fname);
    internal_code code = new_code(root);
    load_node(&code, root);
    f = fopen(buf, "w");
    print_code(f, &code);
    fclose(f);
    return code;
}
