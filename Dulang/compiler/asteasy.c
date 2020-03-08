//
//  asteasy.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 02.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "astgen.h"
#include "compilerr.h"
#include <stdlib.h>
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

void print_astnode(astnode*node, FILE*output, int offset){
    for(int i = 0; i < offset; i++){
        fputc(' ', output);
    }
    fprintf(output, "%s ", asttype_repr[node->type]);
    if( node->type == NUMLIT ){
        fprintf(output, "%lf", *(double*)&node->val);
    }
    if( node->type == STRLIT || node->type == NAME ){
        fprintf(output, "%s", (char*)node->val);
    }
    fprintf(output, " regs needed: %d", node->registers_needed);
    fputc('\n', output);
    for(int i = 0; i < node->childrencount; i++){
        print_astnode(node->children[i], output, offset + 4);
    }
}


astnode * astparse_text(const char * text, int length){
    lexem_array lexems = parse_text(text, length);
    lexem_iterator lexem_walker = {lexems, 0, "temp"};
    lexem current = get_current(&lexem_walker);
    astnode * root = astnode_new(COMPOUND, 100, 0);
    while(!( current.type == NONE )){
        astnode * statement = astparse_stat(&lexem_walker);
        ast_add_child(root, statement);
        current = get_current(&lexem_walker);
    }
    count_register_count(root);
    return root;
    
}
astnode * astparse_file(const char * fname){
    FILE * f = fopen(fname, "r");
    if(!f)
        return NULL;
    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    rewind(f);
    char * text_buf = malloc(fsize + 1);
    fread(text_buf, 1, fsize, f);
    text_buf[fsize] = 0;
    astnode * file_root = astparse_text(text_buf, (int)fsize);
    free(text_buf);
    return file_root;
}

astnode * astparse_stat(lexem_iterator * lwalker){
    lexem first = get_current(lwalker);
    if( first.type == SPECIAL && first.special_type == kwwrite ){
        get_next(lwalker);
        astnode * expression = astparse_expr(lwalker);
        //remove the eol
        get_next(lwalker);
        return astnode_new(WRITE, 1, 1, expression);
    }
    //TODO: same for other key-wordic statements
    astnode * left = astparse_expr(lwalker);
    lexem current = get_current(lwalker);
    if( current.type == SPECIAL && current.special_type == assign ){
        //TODO: assume that left part is assignable
        get_next(lwalker);
        astnode * right = astparse_expr(lwalker);
        //remove the eol
        get_next(lwalker);
        if( !left || !right )
            return 0;
        return astnode_new(ASSIGN, 2, 2, left, right);
    }
    //TODO: do same for inplace ops
    
    //TODO: assume that left is a func call or method call
    if( current.type == SPECIAL && current.special_type == EOL ){
        get_next(lwalker);
        return left;
    }
    
    
    return 0;
}


//TODO: replace it with more complex code
astnode * astparse_expr( lexem_iterator * lwalker ){
    return astparse_plus(lwalker);
}

astnode * astparse_plus( lexem_iterator * lwalker ){
    astnode * left = astparse_mult(lwalker);
    lexem l = get_current(lwalker);
    if( l.type == SPECIAL && (l.special_type == plus || l.special_type == minus) ){
        get_next(lwalker);
        astnode * right = astparse_mult(lwalker);
        if( !left || !right )
            return 0;
        enum astnodetype nodetype = PLUS;
        if( l.special_type == minus ) nodetype = MINUS;
        return astnode_new(nodetype, 2, 2, left, right);
    }
    return left;
}


astnode * astparse_mult( lexem_iterator * lwalker ){
    astnode * left = astparse_unar(lwalker);
    lexem l = get_current(lwalker);
    if( l.type == SPECIAL && (l.special_type ==  asterisk || l.special_type == slash) ){
        get_next(lwalker);
        astnode * right = astparse_mult(lwalker);
        if( !left || !right )
            return 0;
        enum astnodetype nodetype = MULT;
        if( l.special_type == slash ) nodetype = DIV;
        return astnode_new(nodetype, 2, 2, left, right);
    }
    return left;
}


//TODO: parse unary dot, plus and minus
astnode * astparse_unar(lexem_iterator * lwalker){
    return astparse_topl(lwalker);
}
//TODO: parse strings, jsons, subscrs, parenthesis
astnode * astparse_topl(lexem_iterator * lwalker){
    lexem lex = get_current(lwalker);
    get_next(lwalker);
    if( lex.type == IDENTIFIER ){
        astnode* node =  astnode_new(NAME, 0, 0);
        node->val = lex.value;
        return node;
    }
    if( lex.type == NUMLITERAL ){
        astnode * node = astnode_new(NUMLIT, 0, 0);
        node->val = lex.value;
        return node;
    }
    return 0;
}

//TODO: widen type list
int count_register_count(astnode * node){
    switch (node->type) {
        case NUMLIT:
        case STRLIT:
        case NAME:
            node->registers_needed = 0;
            return 0;
            break;
        case PLUS:
        case MINUS:
        case MULT:
        case ASSIGN:
        case DIV:{
            int reg_count = MAX(count_register_count(node->children[0]), count_register_count(node->children[1])) + 1;
            node->registers_needed = reg_count;
            return reg_count;
        }break;
        case COMPOUND:{
            int max_needed = 0;
            for(int i = 0; i < node->childrencount; i++){
                if( count_register_count(node->children[i]) > max_needed ){
                    max_needed = node->children[i]->registers_needed;
                }
            }
            node->registers_needed = max_needed;
            return max_needed;
        }break;
        case WRITE:
            node->registers_needed =count_register_count(node->children[0]);
            return node->registers_needed;
        default:
            break;
    }
    return 0;
}

