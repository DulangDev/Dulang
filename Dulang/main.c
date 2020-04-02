//
//  main.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 11.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include <stdio.h>
#include "dulang.h"
#include "compiler/astgen.h"
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "compiler/asttest.h"
#include "compiler/bytecodegen.h"
#include "Native/macOS/asmexamples.h"
#include <sandbox.h>
void test_lexer(){
    DIR *d;
    struct dirent *dir;
    char namebuf [2000];
    sprintf(namebuf, "%s/Dulang/Dulang/tests", getenv("HOME"));
    char filenamebuf [2000];
    char outtestbuf [2000];
    char dirname [2000];
    sprintf(dirname, "%s/lexer_tests", namebuf);
    d = opendir(dirname);
    int testNo = 0;
    while ((dir = readdir(d)) != NULL) {
        if(strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")){
            sprintf(filenamebuf, "%s/lexer_tests/%s", namebuf, dir->d_name);
            sprintf(outtestbuf, "%s/test_results/lexer_test_%s", namebuf, dir->d_name);
            lexem_array l_arr = parse_file(filenamebuf);
            print_lexems(l_arr, outtestbuf);
        }
        
    }
    closedir(d);
}
static const char kSandboxPolicy[] = "(\n"
"(version 1)\n"
"(deny default)\n"
"(deny dynamic-code-generation)\n"
")";

void enable_sandbox() {
    char *err;
    int rv = sandbox_init(kSandboxPolicy, 0, &err);
    if (rv) {
        fprintf(stderr, "sandbox_init error: %s\n", err);
        exit(1);
    }
}

int main(int argc, const char * argv[]) {

    internal_code c = generate_code_from_source("/Users/jernicozz/Dulang/Dulang/tests/asteasy/main.dul");
    asmwriter writer = create_writer();
    write_vm_code(&c, &writer);
    asmfunc func = generate_from_writer(writer);
    void * stack = malloc(1024);
    printf("compiled %p\n", func);
    
    char * f_repr = (char*)func;
    for(int i = 0; i < 10; i+=4){
        printf("%X ", *(int*)(f_repr+i));
    }
    
    (*func)(c.statics, stack);
  
    
    return 0;
}
