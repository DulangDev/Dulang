//
//  asttype_pred.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 03.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "asttype_pred.h"
void prepare_table_recursively(astnode * node, dul_type * layout){
    node->name_table = (dul_layout_type*)layout;
    for(int i = 0; i < node->childrencount; i++){
        prepare_table_recursively(node->children[i], layout);
    }
}

void prepare_table(astnode * root){
    prepare_table_recursively(root, (dul_type*)create_layout_type());
}

void generate_types(astnode * node){
    if(node->type != ASSIGN)
    for(int i = 0; i < node->childrencount; i++){
        generate_types(node->children[i]);
    }
    else
        generate_types(node->children[1]);
    if(node->type == INTEGERLIT){
        node->node_type = &INT_T;
    }
    if(node->type == NUMLIT){
        node->node_type = &FLOAT_T;
    }
    if(node->type == ASSIGN){
        //TODO: for subscripts
        dul_type * rval_t = node->children[1]->node_type;
        node->children[0]->node_type = node->children[1]->node_type;
        char * varname = (char*)node->children[0]->val;
        struct field f = {
            varname,
            rval_t
        };
        int varstatus = contains_entry(node->name_table, f);
        if(varstatus == LAYOUT_ENTRY_CONTAINS){
            //ok
        }
        if(varstatus == LAYOUT_WRONG_ENTRY_TYPE){
            //TODO: compilation error
        }
        if(varstatus == LAYOUT_NOT_CONTAINS_ENTRY){
            add_entry_to_layout_type(node->name_table, f);
        }
        node->node_type = node->children[0]->node_type;
    }
    if(node->type == NAME){
        char * varname = (char*)node->val;
        struct field f = {
            varname,
            0
        };
        int varstatus = contains_entry(node->name_table, f);
        if(varstatus == LAYOUT_NOT_CONTAINS_ENTRY){
            add_entry_to_layout_type(node->name_table, f);
        }
        if(varstatus == LAYOUT_ENTRY_CONTAINS){
            node->node_type = get_field_type(node->name_table, node->val);
        }
        
        //TODO: compilation error on fail
    }
    if(node->type == PLUS || node->type == MINUS || node->type == MULT || node->type == DIV){
        //types can only be arithmetic
        dul_type * lhs_type = node->children[0]->node_type;
        dul_type * rhs_type = node->children[1]->node_type;
        
        if( !isNumericType(lhs_type->typeid) || !isNumericType(rhs_type->typeid) ){
            //TODO: compilation error
            //TODO: for strings
        }
        if( lhs_type->typeid == FLOAT_ID || rhs_type->typeid == FLOAT_ID ){
            node->node_type = &FLOAT_T;
            if(lhs_type->typeid == INT_ID){
                astnode * proxy = astnode_new(CAST_TO_FLOAT, 1, 1, node->children[0]);
                proxy->node_type = &FLOAT_T;
                node->children[0] = proxy;
            }
            if(rhs_type->typeid == INT_ID){
                astnode * proxy = astnode_new(CAST_TO_FLOAT, 1, 1, node->children[1]);
                proxy->node_type = &FLOAT_T;
                node->children[1] = proxy;
            }
            
        } else {
            node->node_type = &INT_T;
        }
        
    }
    if(node->type == WRITE){
        node->node_type = node->children[0]->node_type;
    }
}
