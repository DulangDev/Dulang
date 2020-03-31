//
//  lexer.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 13.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "lexer.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
static const int INITIAL_ARRAY_CAPACITY = 1000;

static lexem_array init_array(void){
    lexem_array l_arr;
    l_arr.lexems = (lexem*)malloc(sizeof(lexem) * INITIAL_ARRAY_CAPACITY);
    l_arr.lexemcap = INITIAL_ARRAY_CAPACITY;
    l_arr.lexemcount = 0;
    return l_arr;
}

static void add_lexem(lexem_array * array, lexem lex){
    if(!array)
        return;
    if(!array->lexems)
        return;
    if(array->lexemcount ++ == array->lexemcap){
        array->lexemcap *= 2;
        array->lexems = (lexem*)realloc(array->lexems, sizeof(lexem)*array->lexemcap);
    }
    array->lexems[array->lexemcount - 1] = lex;
}

lexem_array parse_file(const char * fname){
    FILE * f = fopen(fname, "r");
    if(!f){
        lexem_array l_arr;
        (void)bzero(&l_arr, sizeof(l_arr));
        return l_arr;
    }
    struct stat buff;
    fstat(fileno(f), &buff);
    long file_size = buff.st_size + 1;
    //allocate buffer for this
    char * buffer = (char*)malloc(file_size);
    fread(buffer, 1, file_size - 1, f);
    buffer[file_size-1] = 0;
    return parse_text(buffer, file_size);
}

int isIdentifyingSymbol(char c){
    return c == '_' || (c <= 'z' && c >= 'a') || (c >= 'A' && c <= 'Z');
}

static void write_str_formatted(long size, const char *str, char *str_writer, const char *string_reader) {
    while( string_reader < str + size && string_reader[0] != '"' ){
        if(*string_reader == '\\'){
            switch (*++string_reader) {
                case 'n':
                    *str_writer++ = '\n';
                    break;
                case 't':
                    *str_writer++ = '\t';
                    break;
                default:
                    *str_writer++ = string_reader[0];
                    break;
            }
            string_reader++;
        } else {
            *str_writer++ = *string_reader++;
        }
    }
}

static lexem scan_string(char **endpos, const char *reader, long size, const char *str) {
    const char * string_reader = reader + 1;
    //firstly lets detect its size:
    while( string_reader < str + size && string_reader[0] != '"' ){
        if(*string_reader == '\\'){
            //this is used to prevent stopping when " is found
            ++string_reader;
        }
        ++string_reader;
    }
    // +1 to add \0 to the end
    int strsize = (int)(string_reader - reader);
    char * str_ = (char*)malloc(strsize + 1);
    string_reader = reader + 1;
    char * str_writer = str_;
    write_str_formatted(size, str, str_writer, string_reader);
    *str_writer = 0;
    lexem l;
    l.type = STRLNGLITERAL;
    l.value = str_;
    *endpos = (char*)string_reader + 1;
    return l;
}

static lexem find_special(char **endpos, int i, const char *reader) {
    lexem l;
    l.type = SPECIAL;
    l.special_type = i;
    l.value = 0;
    *endpos = (char*)reader + strlen(ltext[i]);
    return l;
}

lexem getLexem(const char * str, long size,  char ** endpos){
    const char * reader = str;
    while(*reader == ' '){
        reader ++;
    }
    if(*reader == '"'){
        //starts the string
        return scan_string(endpos, reader, size, str);
    }
    for(int i = 0; i < kw_logical_or; ++i){
        if(strncmp(ltext[i], reader, strlen(ltext[i])) == 0){
            return find_special(endpos, i, reader);
        }
    }
    const char * metareader = reader;
    while(isIdentifyingSymbol(*metareader)){
        metareader++;
    }
    if(metareader != reader){
        //can be still a keyword
        for(int i = kw_logical_or; i < indent; ++i){
            if(metareader - reader == strlen(ltext[i]))
            if(strncmp(ltext[i], reader, metareader - reader) == 0){
                lexem l;
                l.type = SPECIAL;
                l.special_type = i;
                l.value = 0;
                *endpos = (char*)reader + strlen(ltext[i]);
                return l;
            }
        }
        lexem l;
        l.type = IDENTIFIER;
        l.value = strndup(reader, metareader - reader);
        *endpos = (char*)metareader;
        return l;
    }
    if(isdigit(*reader)){
        double number = strtod(reader, (char**)&reader);
        lexem l;
        l.type = NUMLITERAL;
        memcpy(&l.value, &number, sizeof(double));
        if( number == (int64_t)number ){
            l.type = NUMINTLITERAL;
            int64_t i_val = number;
            memcpy(&l.value, &i_val, sizeof(int64_t));
        }
        
        *endpos = (char*)reader;
        return l;
    }
    if(*reader == '\n'){
        lexem l;
        l.type = SPECIAL;
        l.special_type = EOL;
        *endpos = (char*)reader + 1;
        return l;
    }
    lexem l;
    l.type = ERROR;
    return l;
}


lexem_array parse_text(const char * str, long size){
    lexem_array l_arr = init_array();
    const char * reader = str;
    if(*reader == ' '){
        //not so proper code
        lexem err_lexem;
        err_lexem.value = strdup("whitespaces at the beginning of file are not allowed");
        err_lexem.type = ERROR;
        add_lexem(&l_arr, err_lexem);
    }
    while(reader < str + size - 1){
        lexem l = getLexem(reader, size - (reader - str), (char**)&reader);
        //ignores whitespaces/tabs at beginning of line
        l.offset = (int)(reader - str);
        add_lexem(&l_arr, l);
        
    }
    return l_arr;
}

void print_lexems (lexem_array array, const char * fname){
    FILE * f = fopen(fname, "w");
    for(int i = 0; i < array.lexemcount; ++i){
        fprintf(f, "lexem #%.*d: ", (int)log10(array.lexemcount) + 1 ,i);
        switch (array.lexems[i].type) {
            case STRLNGLITERAL:
                fprintf(f, "String[ \"%s\" ]\n", (char*)array.lexems[i].value);
                break;
            case NUMLITERAL:
                fprintf(f, "Number[ %lf ]\n", *(double*)&array.lexems[i].value);
                break;
            case SPECIAL:
                fprintf(f, "Special[ %s ]\n", ltext[array.lexems[i].special_type]);
                break;
            case IDENTIFIER:
                fprintf(f, "Symbol[ %s ]\n", (char*)array.lexems[i].value);
                break;
            default:
                fprintf(f, "ERROR IN PARSING\n");
                break;
        }
    }
    fclose(f);
}
