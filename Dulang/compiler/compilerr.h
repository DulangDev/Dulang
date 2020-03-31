//
//  compilerr.h
//  Dulang
//
//  Created by Дмитрий Маслюков on 16.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef compilerr_h
#define compilerr_h

void log_error_in_file(const char * fname, int offset, const char * errfmt, ...);

typedef struct {
    char desc [256];
    int lineno;
    int linepos;
} compile_error;

#endif /* compilerr_h */
