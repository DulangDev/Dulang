//
//  asteasy.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 02.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "astgen.h"
#include "optimizer.h"
#include "compilerr.h"
#include <stdlib.h>
#include <string.h>
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

void print_astnode(astnode*node, FILE*output, int offset){
    for(int i = 0; i < offset; i++){
        fputc(' ', output);
    }
    fprintf(output, "%s: [type:%s] ", asttype_repr[node->type], node->node_type?node->node_type->basic_name:"undetermined");
    if( node->type == NUMLIT ){
        fprintf(output, "%lf", *(double*)node->val);
    }
    if( node->type == INTEGERLIT ){
        fprintf(output, "%llu", *(uint64_t*)node->val);
    }
    if( node->type == STRLIT || node->type == NAME ){
        fprintf(output, "%s", (char*)node->val);
    }
    fprintf(output, " place needed: %d", node->place_needed);
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
    optimize_statics_math(root);
    prepare_table(root);
    generate_types(root);
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
    if( first.type == SPECIAL && first.special_type == kwif ){
        astnode * ifnode = astnode_new(IFSTAT, 3, 0);
        //
    }
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
    while( l.type == SPECIAL && (l.special_type == plus || l.special_type == minus) ){
        get_next(lwalker);
        astnode * right = astparse_mult(lwalker);
        if( !left || !right )
            return 0;
        enum astnodetype nodetype = PLUS;
        if( l.special_type == minus ) nodetype = MINUS;
        left =  astnode_new(nodetype, 2, 2, left, right);
        l = get_current(lwalker);
    }
    return left;
}


astnode * astparse_mult( lexem_iterator * lwalker ){
    astnode * left = astparse_unar(lwalker);
    lexem l = get_current(lwalker);
    while( l.type == SPECIAL && (l.special_type ==  asterisk || l.special_type == slash) ){
        get_next(lwalker);
        astnode * right = astparse_mult(lwalker);
        if( !left || !right )
            return 0;
        enum astnodetype nodetype = MULT;
        if( l.special_type == slash ) nodetype = DIV;
        left = astnode_new(nodetype, 2, 2, left, right);
        l = get_current(lwalker);
    }
    return left;
}


//TODO: parse unary dot, plus and minus
astnode * astparse_unar(lexem_iterator * lwalker){
    return astparse_topl(lwalker);
}


static astnode * astparse_funcargs(lexem_iterator * walker){
    int args_presented = lexem_occures_till_eol(walker, arrow);
    if(!args_presented){
        return astnode_new(MKTUPLE, 0, 0);
    }
    lexem l = get_current(walker);
    if(l.type != IDENTIFIER){
        return 0;
    }
    return 0;
}

//TODO: parse strings, jsons, subscrs, parenthesis
astnode * astparse_topl(lexem_iterator * lwalker){
    lexem lex = get_current(lwalker);
    get_next(lwalker);
    
    if(lex.type == SPECIAL && lex.special_type == Obrace){
        //TODO: identify json objects
        
        
    }
    
    if( lex.type == IDENTIFIER ){
        astnode* node =  astnode_new(NAME, 0, 0);
        memcpy(&node->val, lex.value, strlen((char*)lex.value));
        node->val[strlen((char*)lex.value)] = 0;
        return node;
    }
    if( lex.type == NUMLITERAL ){
        astnode * node = astnode_new(NUMLIT, 0, 0);
        memcpy(&node->val, &lex.value, sizeof(void*));
        return node;
    }
    if( lex.type == NUMINTLITERAL ){
        astnode * node = astnode_new(INTEGERLIT, 0, 0);
        memcpy(&node->val, &lex.value, sizeof(int64_t));
        return node;
    }
    return 0;
}

//TODO: widen type list
size_t count_register_count(astnode * node){
    switch (node->type) {
        case NUMLIT:
        case STRLIT:
        case NAME:
            node->place_needed = 0;
            return 0;
            break;
        case PLUS:
        case MINUS:
        case MULT:
        case DIV:{
            size_t reg_count = MAX(count_register_count(node->children[0]), count_register_count(node->children[1])) + 8;
            node->place_needed = reg_count;
            return reg_count;
        }break;
        case CAST_TO_FLOAT:
            node->place_needed = 8;
            return 8;
            break;
        case ASSIGN:
            return 0;
            //TODO: think of subsript assign (but it is an offset however)
            // we can think of subscript assign as assign to a special variable
            break;
        case COMPOUND:{
            size_t max_needed = 0;
            for(int i = 0; i < node->childrencount; i++){
                if( count_register_count(node->children[i]) > max_needed ){
                    max_needed = node->children[i]->place_needed;
                }
            }
            node->place_needed = max_needed;
            return max_needed;
        }break;
        case WRITE:
            node->place_needed= count_register_count(node->children[0]);
            return node->place_needed;
        default:
            break;
    }
    return 0;
}


