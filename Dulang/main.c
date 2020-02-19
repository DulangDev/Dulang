//
//  main.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 11.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include <stdio.h>
#include "dulang.h"
#include "lexer.h"
#include "astgen.h"
#include <dirent.h>
#include <stdlib.h>

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

int main(int argc, const char * argv[]) {
    // insert code here...
    test_lexer();
    astnode * main = astparse_file("/Users/jernicozz/Dulang/Dulang/tests/lexer_tests/main.dul");
    return 0;
}
