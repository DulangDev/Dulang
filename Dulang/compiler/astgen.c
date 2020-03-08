//
//  astgen.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 13.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "astgen.h"
#include "compilerr.h"
#include <stdarg.h>

lexem get_current   (lexem_iterator * iter){
    if(iter->currlexem >= iter->lstream.lexemcount){
        lexem ret_l = {NONE, 0, 0, 0};
        return ret_l;
    }
    return iter->lstream.lexems[iter->currlexem];
}

lexem get_next      (lexem_iterator * iter){
    iter->currlexem ++;
    return get_current(iter);
}

lexem get_next_pos  (lexem_iterator * iter, int pos){
    if(iter->currlexem + pos >= iter->lstream.lexemcount){
        lexem ret_l = {NONE, 0, 0, 0};
        return ret_l;
    }
    return iter->lstream.lexems[iter->currlexem + pos];
}


int is_statement_terminating( lexem l ){
    return l.type == NONE || (l.type == SPECIAL && (l.special_type == EOL || l.special_type == Cbrace));
}

astnode * astnode_new  ( enum astnodetype type, int capacity, int children, ... ){
    va_list va;
    va_start(va, children);
    return astnode_val(type, capacity, children, va);
}

void ast_add_child (astnode * parent, astnode * child){
    if(parent->childrencount ++ == parent->childrencap){
        parent->childrencap *= 2;
        parent->children = (astnode**)realloc(parent->children, parent->childrencap);
    }
    parent->children[ parent->childrencount - 1 ] = child;
    child->parent = parent;
}

astnode * astnode_val  ( enum astnodetype type, int capacity, int children, va_list va_args){
    astnode * result = (astnode*)malloc(sizeof(astnode));
    result->type = type;
    result->children = (astnode**)malloc(sizeof(astnode*)*capacity);
    result->childrencap = capacity;
    for(int i = 0; i < children; ++i){
        ast_add_child(result, va_arg(va_args, astnode*));
    }
    return result;
}
astnode * astparse_file_(const char * fname){
    lexem_array array = parse_file(fname);
    if(array.lexems == 0){
        return NULL;
    }
    lexem_iterator iter = {
        array,
        0,
        fname
    };
    return astnode_new(MODULE, 15, 1, astparse_many(&iter));
}

static int is_closing_lexem (lexem l){
    return l.type == NONE || (l.type == SPECIAL && l.special_type == Cbrace);
}

//important: walks with the start of the text: the concern of removing { and end-of-lines lie on handlers
astnode * astparse_many_(lexem_iterator * iter){
    astnode * node = astnode_new(COMPOUND, 300, 0);
    while( !is_closing_lexem( get_current(iter) ) ){
        astnode * n = astparse_stat(iter);
        if (!n){
            return 0;
        }
    }
    return node;
}

static int is_assign_like(lexem_iterator *iter) {
    int is_assign_like = lexem_occures_till_eol(iter, assign) || lexem_occures_till_eol(iter, inpadd) || lexem_occures_till_eol(iter, inpsub) || lexem_occures_till_eol(iter, inpmult) || lexem_occures_till_eol(iter, inpdiv);
    return is_assign_like;
}

static enum astnodetype assign_type(lexem _l) {
    
    if( _l.special_type == assign ){
        return ASSIGN;
    } else if(_l.special_type >= inpadd && _l.special_type <= inpdiv){
       return INPLADD + (_l.special_type - inpadd);
    } else return 0;
}

astnode * astparse_stat_(lexem_iterator * iter){
    lexem curr = get_current(iter);
    if(curr.type == SPECIAL){
        switch (curr.special_type) {
            case EOL:
                return astnode_new(EMPTY_STR, 0, 0);
            case kwreturn:{
                astnode * ret_expr = astparse_expr(iter);
                return astnode_new(DULRETURN, 1, 1, ret_expr);
            }break;
            case kwif:
                break;
            default:
                break;
        }
    }
    if(is_assign_like(iter)){
        astnode * left = astparse_lval(iter);
        lexem _l = get_current(iter);
        get_next(iter);
        astnode * right = astparse_expr(iter);
        enum astnodetype _type = assign_type(_l);
        return astnode_new(_type, 2, 2, left, right);
    }
    log_error_in_file(iter->filename, get_current(iter).offset, "Statement of unsupported kind\n");
    return 0;
}

int lexem_occures_till_eol(lexem_iterator * iter, enum splexems type){
    int pos = -1, is_satisfying = 0;
    lexem l_next;
    do{
        pos++;
        l_next = get_next_pos(iter, pos);
        is_satisfying = l_next.type == SPECIAL && l_next.special_type == type;
        if(is_satisfying){
            return 1;
        }
    } while( !is_statement_terminating(l_next));
    return 0;
}

astnode * astparse_lval_(lexem_iterator*iter){
    return 0;
}
