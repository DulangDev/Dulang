//
//  asttest.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 02.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//


//some tests for ast parsing
#include "asttest.h"

void test_ast_parse(const char * fname){
    astnode * root = astparse_file(fname);
    char buf [1000];
    sprintf(buf, "%s.ast", fname);
    FILE * f = fopen(buf, "w");
    print_astnode(root, f, 0);
    fclose(f);
}
