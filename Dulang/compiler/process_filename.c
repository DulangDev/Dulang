//
//  process_filename.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 16.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "process_filename.h"

char * fname_to_modname(const char * filename){
    char * namebuf = malloc(strlen(filename) + 1);
    strcpy(namebuf, filename);
    size_t name_len = strlen(filename);
    if(name_len > 4){
        if(strcmp(filename + name_len - 4, ".dul") == 0){
            namebuf[name_len - 4] = 0;
        }
    }
    for(int i = 0; i < name_len; ++i){
        if(namebuf[i] == '/'){
            namebuf[i] = '.';
        }
    }
    return namebuf;
}
