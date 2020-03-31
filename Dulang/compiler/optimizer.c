//
//  optimizer.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 24.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "optimizer.h"
#include <string.h>

int is_numeric_literal(astnode * node){
    return node->type == NUMLIT || node->type == INTEGERLIT;
}

static double get_double_val(astnode * node){
    return *(double*)&node->val;
}
static int64_t get_int_val(astnode * node){
    return *(int64_t*)&node->val;
}

static double cast_val_to_double(astnode * node){
    if(node->type == NUMLIT){
        return get_double_val(node);
    } else {
        return (double)get_int_val(node);
    }
}

void optimize_statics_math(astnode * node){
    for(int i = 0; i < node->childrencount; i++){
        optimize_statics_math(node->children[i]);
    }
    if(node->type == MULT || node->type == DIV || node->type == PLUS || node->type == MINUS){
        if(is_numeric_literal(node->children[0]) && is_numeric_literal(node->children[1])){
            //can be optimized
            if(node->children[0]->type == NUMLIT || node->children[1]->type == NUMLIT){
                // one is float, needs cast to double
                double lhs = cast_val_to_double(node->children[0]);
                double rhs = cast_val_to_double(node->children[1]);
               
                
                double res=0;
                switch (node->type) {
                    case PLUS:
                        res = lhs + rhs;
                        break;
                    case MINUS:
                        res = lhs - rhs;
                        break;
                    case MULT:
                        res = lhs * rhs;
                        break;
                    case DIV:
                        res = lhs / rhs;
                        break;
                    default:
                        break;
                }
                 node->type = NUMLIT;
                memcpy(node->val, &res, sizeof(double));
                
            } else {
                //both are integer
                int64_t lhs = get_int_val(node->children[0]);
                int64_t rhs = get_int_val(node->children[1]);
                
                int64_t res;
                switch (node->type) {
                    case PLUS:
                        res = lhs + rhs;
                        break;
                    case MINUS:
                        res = lhs - rhs;
                        break;
                    case MULT:
                        res = lhs * rhs;
                        break;
                    case DIV:
                        res = lhs / rhs;
                    default:
                        break;
                }
                node->type = INTEGERLIT;
                memcpy(node->val, &res, sizeof(int64_t));
            }
            node->childrencount=0;
            destroy_astnode(node->children[0]);
            destroy_astnode(node->children[1]);
            free(node->children);
            
            
        }
        //TODO: for strings
    }
    
    
}
