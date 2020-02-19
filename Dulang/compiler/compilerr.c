//
//  compilerr.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 16.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "compilerr.h"
#include "process_filename.h"
#include <stdio.h>
#include <stdarg.h>

struct linecords{
    int lineno;
    int linepos;
};


static struct linecords count_line_coords(FILE * f, int offset){
    char * mem = (char*)malloc(offset + 1);
    offset = (int)fread(mem, 1, offset, f);
    int lineno = 1;
    int last_line_begin = 0;
    for(int i = 0; i < offset; ++i){
        if( mem[i] == '\n' ){
            lineno++;
            last_line_begin = i;
        }
    }
    free(mem);
    struct linecords cords = {
        lineno,
        offset - last_line_begin
    };
    return cords;
}


static FILE * logfile_for_module(const char * modname){
    char modnamebuf [2000];
    sprintf(modnamebuf, "%s.err", modname);
    FILE * f = fopen(modnamebuf, "a+");
    rewind(f);
    return f;
}


void log_error_in_file(const char * fname, int offset, const char * errdesc, ...){
    char * modname = fname_to_modname(fname);
    FILE * f = logfile_for_module(modname);
    struct linecords cords = count_line_coords(f, offset);
    va_list va;
    va_start(va, errdesc);
    
    
    fprintf(f, "Error in %d:%d:\n", cords.lineno, cords.linepos);
    vfprintf(f, errdesc, va);
    
    
    va_end(va);
    free(modname);
    fclose(f);
}
