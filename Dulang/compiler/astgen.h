//
//  astgen.h
//  Dulang
//
//  Created by Дмитрий Маслюков on 13.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef astgen_h
#define astgen_h

#include <stdlib.h>
#include <stdio.h>
#include "lexer.h"
#include "../objects/type.h"
#include <stdarg.h>

enum astnodetype{
    SUBSCR,
    COMPOUND,
    ASSIGN,
    INPLADD,
    INPLSUB,
    INPLMULT,
    INPLDIV,
    MKTUPLE,
    LESSTHAN,
    LESSEQ,
    GREATERTHAN,
    GREATEREQ,
    EQUAL,
    NEQUAL,
    LOGOR,
    LOGAND,
    LOGNOT,
    FUNCCALL,
    FUNCDEF,
    WRITE,
    IFSTAT,
    PLUS,
    MINUS,
    MULT,
    DIV,
    NUMLIT,
    STRLIT,
    BOOLLIT,
    NAME,
    INOP,
    FORSTAT,
    ASYNC,
    NULL_,
    DULRETURN,
    MODULE,
    UNPACK_ITER,
    IMPORT,
    WHILE,
    IS,
    EMPTY_STR,
    INTEGERLIT,
    CAST_TO_FLOAT
};
static const char * asttype_repr [] = {
        "SUBSCR",
        "COMPOUND",
        "ASSIGN",
        "INPLADD",
        "INPLSUB",
        "INPLMULT",
        "INPLDIV",
        "MKTUPLE",
        "LESSTHAN",
        "LESSEQ",
        "GREATERTHAN",
        "GREATEREQ",
        "EQUAL",
        "NEQUAL",
        "LOGOR",
        "LOGAND",
        "LOGNOT",
        "FUNCCALL",
        "FUNCDEF",
        "WRITE",
        "IFSTAT",
        "PLUS",
        "MINUS",
        "MULT",
        "DIV",
        "NUMLIT",
        "STRLIT",
        "BOOLLIT",
        "NAME",
        "INOP",
        "FORSTAT",
        "ASYNC",
        "NULL_",
        "DULRETURN",
        "MODULE",
        "UNPACK_ITER",
        "IMPORT",
        "WHILE",
        "IS",
        "EMPTY_STR",
        "INTEGER LITERAL",
        "CAST_TO_FLOAT"
};



typedef struct _an{
    enum astnodetype type;
    char val [16];
    struct _an ** children;
    struct _an * parent;
    size_t offset;
    size_t place_needed;
    int childrencount;
    int childrencap;
    
    dul_layout_type * name_table;
    dul_type * node_type;
    
} astnode;

typedef struct {
    lexem_array lstream;
    int currlexem;
    const char * filename;
} lexem_iterator;

lexem get_current   (lexem_iterator * iter);
lexem get_next      (lexem_iterator * iter);

int lexem_occures_till_eol(lexem_iterator * iter, enum splexems type);
void ast_add_child (astnode * parent, astnode * child);

astnode * astnode_new  ( enum astnodetype type, int capacity, int children, ... );
astnode * astnode_val  ( enum astnodetype type, int capacity, int children, va_list );
astnode * astparse_text(const char * text, int length);
astnode * astparse_file(const char * fname);
//either {->} or func syntax
astnode * astparse_modl(lexem_iterator*);
astnode * astparse_func(lexem_iterator*);
astnode * astparse_many(lexem_iterator*);
astnode * astparse_stat(lexem_iterator*);

astnode * astparset_stat_as_assign    (lexem_iterator*);
astnode * astparse_lval(lexem_iterator*);
astnode * astparse_subs(lexem_iterator*);

astnode * astparse_stat_as_definition (lexem_iterator*);


astnode * astparse_expr(lexem_iterator*);
astnode * astparse_def_(lexem_iterator*);
astnode * parse_funcarg(lexem_iterator*);
//and all inpls

astnode * astparse_lgor(lexem_iterator*);
astnode * astparse_land(lexem_iterator*);
astnode * astparse_lnot(lexem_iterator*);
astnode * astparse_inop(lexem_iterator*);

//use typeof instead
//astnode * astparse_isop(lexem_iterator*);

astnode * astparse_comp(lexem_iterator*);
astnode * astparse_plus(lexem_iterator*);
astnode * astparse_mult(lexem_iterator*);
astnode * astparse_unar(lexem_iterator*);
astnode * astparse_post(lexem_iterator*);
astnode * astparse_topl(lexem_iterator*);
astnode * astparse_json(lexem_iterator*);
astnode * astparse_sube(lexem_iterator*, enum splexems close);

astnode * astparse_xml_(lexem_iterator*);

size_t count_register_count(astnode * node);

void print_astnode(astnode*, FILE*, int offset);

void destroy_astnode(astnode * node);
void prepare_table(astnode * root);
void generate_types(astnode * root);
int is_numeric_literal(astnode * node);
#endif /* astgen_h */
