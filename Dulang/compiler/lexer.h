//
//  lexer.h
//  Dulang
//
//  Created by Дмитрий Маслюков on 13.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef lexer_h
#define lexer_h


// lexem is a structural unit of source code. The first step to compile is to split
// stream of bytes (e.g. file with .dul extension or any string) into lexems
// in order to specify proper syntax lexems are required
enum lexemtype {
    IDENTIFIER,
    STRLNGLITERAL,
    NUMLITERAL,
    SPECIAL,
    NONE,
    ERROR
};

enum splexems{
    arrow,
    EOL,
    dot,
    ORBracket,
    CRBracket,
    Obrace,
    Cbrace,
    OSbracket,
    CSbracket,
    comma,
    colon,
    equal,
    nequal,
    lt,
    gt,
    le,
    ge,
    inpadd,
    inpsub,
    inpmult,
    inpdiv,
    assign,
    plus,
    minus,
    deglex,
    asterisk,
    slash,
    kw_logical_or,
    kw_logical_and,
    kw_logical_not,
    kwif,
    kwfor,
    kwtrue,
    kwfalse,
    kwelse,
    kwfunc,
    kwclass,
    kwasync,
    kwwrite,
    kwimport,
    kwnull,
    kwin,
    kwreturn,
    kwwhile,
    indent,
    dedent
};

//not all of this are used in this version of language.

//lexems tagged with $ are represented with their description, not formal text appearance

static const char* ltext[] = {"->","$eol",".", "(", ")", "{", "}", "[", "]", ",", ":", "==", "!=", "<", ">", "<=", ">=", "+=", "-=", "*=", "/=", "=", "+", "-", "**", "*", "/", "or", "and", "not", "if", "for", "true", "false", "else", "fun", "$class", "async", "write", "import", "null", "in", "return", "while", "$indent", "$dedent"
};

typedef struct {
    enum lexemtype type;
    enum splexems special_type;
    void * value;
    int offset;
} lexem;



typedef struct {
    lexem * lexems;
    int lexemcount;
    int lexemcap;
} lexem_array;

lexem_array parse_file(const char * fname);
//if file not opened, lexems is NULL

lexem_array parse_text(const char * str, long size);

//is used for testing and debugging parser
void print_lexems (lexem_array array, const char * fname);


#endif /* lexer_h */
